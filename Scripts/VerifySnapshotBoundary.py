"""Check the native rollover producer/consumer boundary without launching Skyrim."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]


def code(path):
    text = (ROOT / path).read_text(encoding="utf-8-sig")
    # Ignore comments and literals so diagnostic text does not look like a call.
    return re.sub(
        r'R"\([^)]*\)"|"(?:\\.|[^"\\])*"|//[^\n]*|/\*[\s\S]*?\*/',
        lambda match: " " * len(match[0]),
        text,
    )


def body(text, signature):
    start = text.index("{", text.index(signature))
    depth = 1
    end = start + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


errors = []
for path in ("src/Scaleform.cpp", "src/AHZScaleform.cpp"):
    consumer = code(path)
    if path == "src/AHZScaleform.cpp":
        # This API reads only the player; it never derives or resolves the current target.
        consumer = consumer.replace(body(consumer, "void CAHZScaleform::ProcessPlayerData"), "")
    forbidden = re.search(
        r"GetSingleton\s*\(|LookupBy\w*\s*\(|LookupReferenceByHandle\s*\("
        r"|CAHZ(?:Armor|Weapon|Actor)Info::|CAHZPlayerInfo::GetItemAmount\s*\("
        r"|CompletionistAPI::Get\w*\s*\("
        r"|PapyrusMoreHud::(?:GetFormIcons|HasForm)\s*\(",
        consumer,
    )
    if forbidden:
        errors.append(f"{path}: UI consumer reads engine data: {forbidden[0]}")

events = code("src/Events.cpp")
crosshair_event = body(events, "EventResult CrosshairHandler::ProcessEvent")
vr_lookup = body(events, "bool Hook_WandLookupREFRByHandle_Impl")
allowed = crosshair_event + vr_lookup
outside_hooks = events.replace(crosshair_event, "").replace(vr_lookup, "")
if re.search(r"(?:\.|->)crosshairRef\b|LookupReferenceByHandle\s*\(", outside_hooks):
    errors.append("Crosshair target accessed outside the authorized hooks")
all_calls = re.findall(r"\.SetTarget\s*\([^;]*;", events)
allowed_calls = re.findall(r"\.SetTarget\s*\([^;]*;", allowed)
if all_calls != allowed_calls:
    errors.append("SetTarget called outside the authorized crosshair hooks")
for path in (ROOT / "src").glob("*.cpp"):
    if path.name not in ("Events.cpp", "AHZTarget.cpp") and re.search(
        r"\bSetTarget\s*\(", code(path.relative_to(ROOT))
    ):
        errors.append(f"{path.name}: unexpected SetTarget caller")

header = code("include/AHZTarget.h")
if not re.search(r"\bTargetData\s+GetTarget\s*\(\)\s+const", header) or "m_snapshotMutex" not in body(
    header, "GetTarget() const"
):
    errors.append("Snapshot getter must return an owning copy under the publication mutex")
snapshot = body(header, "struct TargetData").split("void Dump", 1)[0]
if re.search(r"\*|NiPointer|ObjectRefHandle|BSFixedString|GFxValue", snapshot):
    errors.append("TargetData contains a non-owning or engine object member")
enemy_snapshot = body(code("include/AHZActorInfo.h"), "class CAHZActorData")
if re.search(r"\*|NiPointer|ObjectRefHandle|BSFixedString|GFxValue", enemy_snapshot):
    errors.append("Enemy snapshot contains a non-owning or engine object member")
target = code("src/AHZTarget.cpp")
papyrus = code("include/AHZPapyrusMoreHud.h")
if "GetFormIcons" in papyrus.split("private:", 1)[0]:
    errors.append("Engine form-list icon queries must be private to target derivation")
if len(re.findall(r"\bCaptureRolloverData\s*\(", target)) != 2 or not re.search(
    r"\bCaptureRolloverData\s*\(", body(target, "void CAHZTarget::DeriveTarget")
):
    errors.append("Rollover capture must be called only by hook-local derivation")
entry = body(target, "void CAHZTarget::SetTarget")
if "CAHZTarget derivation" not in entry or len(re.findall(r"\bDeriveTarget\s*\(", target)) != 2:
    errors.append("Borrowed target state must be derived in a local context")

cleanup = body(body(target, "void CAHZTarget::DeriveTarget"), "clearTransientTargetState")
for pointer in ("m_pObjectRef", "m_pForm", "m_IngredientItem", "m_AlchemyItem", "m_SpellItem"):
    if not re.search(rf"{pointer}\s*=\s*nullptr", cleanup):
        errors.append(f"Transient target state is not cleared: {pointer}")

if errors:
    print("\n".join(errors), file=sys.stderr)
    sys.exit(1)
print("Snapshot boundary passed: UI consumers, hook callers, owning data and transient cleanup")
