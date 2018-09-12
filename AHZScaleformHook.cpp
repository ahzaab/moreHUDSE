#include "skse64/GameTypes.h"
#include "skse64/GameMenus.h"
#include "skse64/ScaleFormTypes.h"
#include "skse64/ScaleFormCallbacks.h"
#include "skse64/GameRTTI.h"
#include "skse64/ScaleFormMovie.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/xbyak/xbyak.h"
#include "AHZScaleformHook.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusEvents.h"
#include <string>
#include "xbyak/xbyak.h"
#include "skse64/skse64_common/BranchTrampoline.h"

using namespace std;
static bool ahzMenuLoaded = false;
static SafeEnemyLevelDataHolder ahzTargetHandle;
TESObjectREFR *g_ahzTargetReference;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
   string menuName(evn->menuName.data);

   if ((ahzMenuLoaded == false) && (menuName == "HUD Menu") && (evn->opening))
   {
      GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);
      HUDMenu *hudMenu = dynamic_cast<HUDMenu*>(MenuManager::GetSingleton()->GetMenu(&evn->menuName));
      if (view)
      {
         GFxValue hudComponent;
         GFxValue result;
         GFxValue args[2];

         if (!view)
         {
            _ERROR("The IMenu returned NULL. The moreHUD widgets will not be loaded.");
         }

         args[0].SetString("AHZWidgetContainer");
         view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
         view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

         if (!hudComponent.objectInterface)
         {
            _ERROR("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded.");
            return EventResult::kEvent_Abort;
         }

         args[0].SetString("AHZHudInfo.swf");
         hudComponent.Invoke("loadMovie", &result, &args[0], 1);
         ahzMenuLoaded = true;
         return EventResult::kEvent_Abort;
      }
   }

   return EventResult::kEvent_Continue;
}

EventResult AHZCrosshairRefEventHandler::ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher)
{
   g_ahzTargetReference = evn->crosshairRef;
   return EventResult::kEvent_Continue;
}

//Unpacked
//HxD Raw 49 8B CA E8 48 6F 64 00 48 8D 4D F0 E8 0F 6D 65 00 48 8D 4D 30 E8 06 6D 65 00 66 44 89 65 70 4C    8814A0
//HxD Raw 49 8B CA E8 E8 6F 64 00 48 8D 4D F0 E8 AF 6D 65 00 48 8D 4D 30 E8 A6 6D 65 00 66 44 89 65 70 4C -->1.5.39
//CFF Explorer .text  53 20 48 8B 4B 10 E8 B5 67 64 00 84 C0 0F 84 F4 00 00 00 8B 43 18 24 8F 3C 08 0F 94 C0 88 44 24
//ida 48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 18 FF FF FF 48 81 EC B0 01 00 00 48 C7 45 A8
uintptr_t Enemy_Update_Hook_Base = 0x00882710;
//.text:00007FF62B5220A0
//.text : 00007FF62B5220A0; == == == == == == == = S U B R O U T I N E == == == == == == == == == == == == == == == == == == == =
//.text:00007FF62B5220A0
//.text : 00007FF62B5220A0; Attributes: bp - based frame fpd = 118h
//.text : 00007FF62B5220A0
//.text : 00007FF62B5220A0; void __usercall EnemyHealth::Update(EnemyHealth *a1@<rcx>, float a2@<xmm0>)
//.text:00007FF62B5220A0                         EnemyHealth__Update proc near; DATA XREF : .rdata : 00007FF62C36B9C8↓o
//.text : 00007FF62B5220A0;.rdata:stru_7FF62C889C1C↓o ...
//.text : 00007FF62B5220A0
//.text : 00007FF62B5220A0                         var_1C0 = qword ptr - 1C0h
//.text : 00007FF62B5220A0                         var_1B8 = qword ptr - 1B8h
//.text : 00007FF62B5220A0                         var_1B0 = byte ptr - 1B0h
//.text : 00007FF62B5220A0                         var_1A0 = qword ptr - 1A0h
//.text : 00007FF62B5220A0                         var_198 = dword ptr - 198h
//.text : 00007FF62B5220A0                         var_190 = qword ptr - 190h
//.text : 00007FF62B5220A0                         var_188 = dword ptr - 188h
//.text : 00007FF62B5220A0                         var_184 = dword ptr - 184h
//.text : 00007FF62B5220A0                         var_180 = dword ptr - 180h
//.text : 00007FF62B5220A0                         var_178 = qword ptr - 178h
//.text : 00007FF62B5220A0                         var_170 = qword ptr - 170h
//.text : 00007FF62B5220A0                         var_168 = byte ptr - 168h
//.text : 00007FF62B5220A0                         var_140 = qword ptr - 140h
//.text : 00007FF62B5220A0                         var_108 = byte ptr - 108h
//.text : 00007FF62B5220A0                         var_C8 = byte ptr - 0C8h
//.text : 00007FF62B5220A0                         var_A0 = byte ptr - 0A0h
//.text : 00007FF62B5220A0                         var_88 = word ptr - 88h
//.text : 00007FF62B5220A0                         var_40 = xmmword ptr - 40h
//.text : 00007FF62B5220A0                         var_28 = dword ptr - 28h
//.text : 00007FF62B5220A0                         TESObjectREFR_ptr = qword ptr - 20h
//.text : 00007FF62B5220A0                         var_18 = qword ptr - 18h
//.text : 00007FF62B5220A0                         arg_18 = qword ptr  28h
//.text : 00007FF62B5220A0
//.text : 00007FF62B5220A0; FUNCTION CHUNK AT.text:00007FF62C0CA5AF SIZE 000000C2 BYTES
//.text : 00007FF62B5220A0
//.text : 00007FF62B5220A0 48 8B C4                                mov     rax, rsp
//.text : 00007FF62B5220A3 55                                      push    rbp
//.text : 00007FF62B5220A4 56                                      push    rsi
//.text : 00007FF62B5220A5 57                                      push    rdi
//.text : 00007FF62B5220A6 41 54                                   push    r12
//.text : 00007FF62B5220A8 41 55                                   push    r13
//.text : 00007FF62B5220AA 41 56                                   push    r14
//.text : 00007FF62B5220AC 41 57                                   push    r15
//.text : 00007FF62B5220AE 48 8D A8 18 FF FF FF                    lea     rbp, [rax - 0E8h]
//.text : 00007FF62B5220B5 48 81 EC B0 01 00 00                    sub     rsp, 1B0h
//.text : 00007FF62B5220BC 48 C7 45 A8 FE FF FF FF                 mov[rbp + 118h + var_170], 0FFFFFFFFFFFFFFFEh
//.text : 00007FF62B5220C4 48 89 58 20                             mov[rax + 20h], rbx
//.text : 00007FF62B5220C8 0F 29 70 B8                             movaps  xmmword ptr[rax - 48h], xmm6
//.text : 00007FF62B5220CC 48 8B F1                                mov     rsi, rcx
//.text : 00007FF62B5220CF 45 33 ED                                xor     r13d, r13d
//.text : 00007FF62B5220D2 4C 89 AD F8 00 00 00                    mov[rbp + 118h + TESObjectREFR_ptr], r13
//.text : 00007FF62B5220D9
//.text : 00007FF62B5220D9                         loc_7FF62B5220D9 : ; DATA XREF : .rdata : 00007FF62C889C24↓o
//.text : 00007FF62B5220D9 48 83 C1 28                             add     rcx, 28h
//.text : 00007FF62B5220DD 48 8D 95 F8 00 00 00                    lea     rdx, [rbp + 118h + TESObjectREFR_ptr]
//.text : 00007FF62B5220E4 E8 37 09 8B FF                          call    LookupREFRByHandle
//.text:00007FF62B5220E9 90                                      nop
//.text : 00007FF62B5220EA
//.text : 00007FF62B5220EA                         loc_7FF62B5220EA : ; DATA XREF : .rdata : 00007FF62C889C2C↓o
//.text : 00007FF62B5220EA 48 8B 05 77 AD 6C 02                    mov     rax, cs : g_ThePlayer
//.text : 00007FF62B5220F1 8B 90 BC 09 00 00                       mov     edx, [rax + 9BCh]
//.text:00007FF62B5220F7 89 95 F0 00 00 00                       mov[rbp + 118h + var_28], edx
//.text : 00007FF62B5220FD 4C 89 AD 00 01 00 00                    mov[rbp + 118h + var_18], r13
//.text : 00007FF62B522104
//.text : 00007FF62B522104                         loc_7FF62B522104 : ; DATA XREF : .rdata : 00007FF62C889C34↓o
//.text : 00007FF62B522104 48 8D 95 00 01 00 00                    lea     rdx, [rbp + 118h + var_18]
//.text : 00007FF62B52210B 48 8D 8D F0 00 00 00                    lea     rcx, [rbp + 118h + var_28]
//.text : 00007FF62B522112 E8 59 11 99 FF                          call    sub_7FF62AEB3270
//.text : 00007FF62B522117 90                                      nop
//.text : 00007FF62B522118
//.text : 00007FF62B522118                         loc_7FF62B522118 : ; DATA XREF : .rdata : 00007FF62C889C3C↓o
//.text : 00007FF62B522118 48 8B 9D F8 00 00 00                    mov     rbx, [rbp + 118h + TESObjectREFR_ptr]
//.text : 00007FF62B52211F 48 8B BD 00 01 00 00                    mov     rdi, [rbp + 118h + var_18]
//.text : 00007FF62B522126 48 85 FF                                test    rdi, rdi
//.text : 00007FF62B522129 74 05                                   jz      short loc_7FF62B522130
//.text : 00007FF62B52212B 4C 8B F7                                mov     r14, rdi
//.text : 00007FF62B52212E EB 10                                   jmp     short loc_7FF62B522140
//.text : 00007FF62B522130; -------------------------------------------------------------------------- -
//.text:00007FF62B522130
//.text : 00007FF62B522130                         loc_7FF62B522130 : ; CODE XREF : EnemyHealth__Update + 89↑j
//.text : 00007FF62B522130 4D 8B F5                                mov     r14, r13
//.text : 00007FF62B522133 48 85 DB                                test    rbx, rbx
//.text : 00007FF62B522136 74 08                                   jz      short loc_7FF62B522140
//.text:00007FF62B522138 80 7B 1A 3E                             cmp     byte ptr[rbx + 1Ah], 3Eh
//.text : 00007FF62B52213C 4C 0F 44 F3                             cmovz   r14, rbx
//.text : 00007FF62B522140
//.text : 00007FF62B522140                         loc_7FF62B522140 : ; CODE XREF : EnemyHealth__Update + 8E↑j
//.text : 00007FF62B522140; EnemyHealth__Update + 96↑j
//.text:00007FF62B522140 49 83 CF FF or r15, 0FFFFFFFFFFFFFFFFh
//.text : 00007FF62B522144 4D 85 F6                                test    r14, r14
//.text : 00007FF62B522147 0F 84 5A 04 00 00                       jz      loc_7FF62B5225A7
//.text : 00007FF62B52214D 49 8B 46 60                             mov     rax, [r14 + 60h]
//.text : 00007FF62B522151 48 85 C0                                test    rax, rax
//.text : 00007FF62B522154 0F 84 4D 04 00 00                       jz      loc_7FF62B5225A7
//.text : 00007FF62B52215A 80 78 46 00                             cmp     byte ptr[rax + 46h], 0
//.text : 00007FF62B52215E 0F 85 43 04 00 00                       jnz     loc_7FF62B5225A7
//.text : 00007FF62B522164 49 8B 06                                mov     rax, [r14]
//.text : 00007FF62B522167 33 D2                                   xor     edx, edx
//.text : 00007FF62B522169 49 8B CE                                mov     rcx, r14
//.text : 00007FF62B52216C FF 90 C8 04 00 00                       call    qword ptr[rax + 4C8h]
//.text : 00007FF62B522172 84 C0                                   test    al, al
//.text : 00007FF62B522174 0F 85 2D 04 00 00                       jnz     loc_7FF62B5225A7
//.text : 00007FF62B52217A 41 8D 57 19                             lea     edx, [r15 + 19h]
//.text : 00007FF62B52217E 49 8B CE                                mov     rcx, r14
//.text : 00007FF62B522181 E8 BA 41 D5 FF                          call    sub_7FF62B276340
//.text : 00007FF62B522186 0F 28 F0                                movaps  xmm6, xmm0
//.text : 00007FF62B522189 0F 57 C9                                xorps   xmm1, xmm1
//.text : 00007FF62B52218C 0F 2F F1                                comiss  xmm6, xmm1
//.text : 00007FF62B52218F 76 28                                   jbe     short loc_7FF62B5221B9
//.text : 00007FF62B522191 F3 0F 10 05 7F 7D CB 00                 movss   xmm0, cs : dword_7FF62C1D9F18
//.text : 00007FF62B522199 F3 0F 59 F0                             mulss   xmm6, xmm0
//.text : 00007FF62B52219D 0F 2F F0                                comiss  xmm6, xmm0
//.text : 00007FF62B5221A0 76 05                                   jbe     short loc_7FF62B5221A7
//.text : 00007FF62B5221A2 0F 28 F0                                movaps  xmm6, xmm0
//.text : 00007FF62B5221A5 EB 1B                                   jmp     short loc_7FF62B5221C2
//.text : 00007FF62B5221A7; -------------------------------------------------------------------------- -
//.text:00007FF62B5221A7
//.text : 00007FF62B5221A7                         loc_7FF62B5221A7 : ; CODE XREF : EnemyHealth__Update + 100↑j
//.text : 00007FF62B5221A7 F3 0F 10 05 79 A1 CB 00                 movss   xmm0, cs : dword_7FF62C1DC328
//.text : 00007FF62B5221AF 0F 2F F0                                comiss  xmm6, xmm0
//.text : 00007FF62B5221B2 73 05                                   jnb     short loc_7FF62B5221B9
//.text : 00007FF62B5221B4 0F 28 F0                                movaps  xmm6, xmm0
//.text : 00007FF62B5221B7 EB 09                                   jmp     short loc_7FF62B5221C2
//.text:00007FF62B5221B9; -------------------------------------------------------------------------- -
//.text:00007FF62B5221B9
//.text : 00007FF62B5221B9                         loc_7FF62B5221B9 : ; CODE XREF : EnemyHealth__Update + EF↑j
//.text : 00007FF62B5221B9; EnemyHealth__Update + 112↑j
//.text:00007FF62B5221B9 0F 2E F1                                ucomiss xmm6, xmm1
//.text : 00007FF62B5221BC 0F 84 E5 03 00 00                       jz      loc_7FF62B5225A7
//.text : 00007FF62B5221C2
//.text : 00007FF62B5221C2                         loc_7FF62B5221C2 : ; CODE XREF : EnemyHealth__Update + 105↑j
//.text : 00007FF62B5221C2; EnemyHealth__Update + 117↑j
//.text:00007FF62B5221C2 80 BE 80 00 00 00 00                    cmp     byte ptr[rsi + 80h], 0
//.text : 00007FF62B5221C9 0F 84 26 02 00 00                       jz      loc_7FF62B5223F5
//.text : 00007FF62B5221CF 8B 05 3B F7 70 02                       mov     eax, cs : dword_7FF62DC31910
//.text : 00007FF62B5221D5 89 46 2C                                mov[rsi + 2Ch], eax
//.text : 00007FF62B5221D8 F3 0F 11 76 30                          movss   dword ptr[rsi + 30h], xmm6
//.text : 00007FF62B5221DD 4C 8D 25 1C A2 C9 FF                    lea     r12, callbeforeenemy
//.text : 00007FF62B5221E4 4C 89 64 24 20                          mov[rsp + 1E0h + var_1C0], r12; void(__cdecl *)(void *)
//.text:00007FF62B5221E9 4C 8D 0D 00 96 C9 FF                    lea     r9, sub_7FF62B1BB7F0; void(__cdecl *)(void *)
//.text:00007FF62B5221F0 BA 18 00 00 00                          mov     edx, 18h; unsigned __int64
//.text:00007FF62B5221F5 44 8D 42 E9                             lea     r8d, [rdx - 17h]; unsigned __int64
//.text:00007FF62B5221F9 48 8D 4C 24 78                          lea     rcx, [rsp + 1E0h + var_168]; void *
//.text:00007FF62B5221FE E8 61 F9 AD 00                          call ? ? _L@YAXPEAX_K1P6AX0@Z2@Z; `eh vector constructor iterator'(void *,unsigned __int64,unsigned __int64,void (*)(void *),void (*)(void *))
//.text:00007FF62B522203 90                                      nop
//.text : 00007FF62B522204
//.text : 00007FF62B522204                         loc_7FF62B522204 : ; DATA XREF : .rdata : 00007FF62C889C44↓o
//.text : 00007FF62B522204 0F 57 C9                                xorps   xmm1, xmm1
//.text : 00007FF62B522207 F3 0F 5A CE                             cvtss2sd xmm1, xmm6
//.text : 00007FF62B52220B 48 8D 4C 24 78                          lea     rcx, [rsp + 1E0h + var_168]
//.text : 00007FF62B522210 E8 2B 77 FD FF                          call    sub_7FF62B4F9940
//.text : 00007FF62B522215 8B 46 40                                mov     eax, [rsi + 40h]
//.text : 00007FF62B522218 24 8F                                   and     al, 8Fh
//.text : 00007FF62B52221A 3C 08                                   cmp     al, 8
//.text:00007FF62B52221C 0F 94 C0                                setz    al
//.text : 00007FF62B52221F 88 44 24 30                             mov[rsp + 1E0h + var_1B0], al
//.text : 00007FF62B522223 48 C7 44 24 28 01 00 00 + mov[rsp + 1E0h + var_1B8], 1
//.text : 00007FF62B52222C 48 8D 44 24 78                          lea     rax, [rsp + 1E0h + var_168]
//.text : 00007FF62B522231 48 89 44 24 20                          mov[rsp + 1E0h + var_1C0], rax
//.text : 00007FF62B522236 4C 8D 0D 13 9E E4 00                    lea     r9, aSetpercent; "SetPercent"
//.text:00007FF62B52223D 45 33 C0                                xor     r8d, r8d
//.text : 00007FF62B522240 48 8B 56 48                             mov     rdx, [rsi + 48h]
//.text : 00007FF62B522244 48 8B 4E 38                             mov     rcx, [rsi + 38h]
//.text : 00007FF62B522248 E8 23 7D 64 00                          call    sub_7FF62BB69F70
//.text : 00007FF62B52224D 8B 46 58                                mov     eax, [rsi + 58h]
//.text : 00007FF62B522250 24 8F                                   and     al, 8Fh
//.text : 00007FF62B522252 3C 08                                   cmp     al, 8
//.text:00007FF62B522254 0F 85 79 01 00 00                       jnz     loc_7FF62B5223D3
//.text : 00007FF62B52225A 8B 46 70                                mov     eax, [rsi + 70h]
//.text : 00007FF62B52225D 24 8F                                   and     al, 8Fh
//.text : 00007FF62B52225F 3C 08                                   cmp     al, 8
//.text:00007FF62B522261 0F 85 6C 01 00 00                       jnz     loc_7FF62B5223D3
//.text : 00007FF62B522267 48 8D 4D 10                             lea     rcx, [rbp + 118h + var_108]
//.text : 00007FF62B52226B E8 50 77 65 00                          call    sub_7FF62BB799C0
//.text : 00007FF62B522270 48 8D 4D 50                             lea     rcx, [rbp + 118h + var_C8]
//.text : 00007FF62B522274 E8 47 77 65 00                          call    sub_7FF62BB799C0
//.text : 00007FF62B522279 66 44 89 AD 90 00 00 00                 mov[rbp + 118h + var_88], r13w
//.text : 00007FF62B522281 4C 8D 45 B0                             lea     r8, [rbp + 118h + var_168]
//.text : 00007FF62B522285 48 8B 56 60                             mov     rdx, [rsi + 60h]
//.text : 00007FF62B522289 48 8B 4E 50                             mov     rcx, [rsi + 50h]
//.text : 00007FF62B52228D E8 9E 70 64 00                          call    sub_7FF62BB69330
//.text : 00007FF62B522292 66 83 8D 90 00 00 00 20 or [rbp + 118h + var_88], 20h
//.text : 00007FF62B52229A F2 0F 10 05 5E 9F E4 00                 movsd   xmm0, cs : qword_7FF62C36C200
//.text : 00007FF62B5222A2 F2 0F 11 45 D8                          movsd[rbp + 118h + var_140], xmm0
//.text : 00007FF62B5222A7 4C 8D 45 B0                             lea     r8, [rbp + 118h + var_168]
//.text : 00007FF62B5222AB 48 8B 56 60                             mov     rdx, [rsi + 60h]
//.text : 00007FF62B5222AF 48 8B 4E 50                             mov     rcx, [rsi + 50h]
//.text : 00007FF62B5222B3 E8 08 92 64 00                          call    sub_7FF62BB6B4C0
//.text:00007FF62B5222B8 49 8B CE                                mov     rcx, r14
//.text : 00007FF62B5222BB E8 B0 40 A1 FF                          call    sub_7FF62AF36370
//.text : 00007FF62B5222C0 49 8B CF                                mov     rcx, r15
//.text : 00007FF62B5222C3
//.text : 00007FF62B5222C3                         loc_7FF62B5222C3 : ; CODE XREF : EnemyHealth__Update + 22A↓j
//.text : 00007FF62B5222C3 48 FF C1                                inc     rcx
//.text : 00007FF62B5222C6 80 3C 08 00                             cmp     byte ptr[rax + rcx], 0
//.text:00007FF62B5222CA 75 F7                                   jnz     short loc_7FF62B5222C3
//.text : 00007FF62B5222CC B8 32 00 00 00                          mov     eax, 32h
//.text : 00007FF62B5222D1 3B C8                                   cmp     ecx, eax
//.text : 00007FF62B5222D3 0F 47 C8                                cmova   ecx, eax
//.text : 00007FF62B5222D6 8D 41 05                                lea     eax, [rcx + 5]
//.text : 00007FF62B5222D9 0F 57 C0                                xorps   xmm0, xmm0
//.text : 00007FF62B5222DC F3 48 0F 2A C0                          cvtsi2ss xmm0, rax
//.text : 00007FF62B5222E1 F3 0F 59 05 1F 9F E4 00                 mulss   xmm0, cs : dword_7FF62C36C208
//.text : 00007FF62B5222E9 F3 4C 0F 2C C0                          cvttss2si r8, xmm0
//.text : 00007FF62B5222EE 41 B1 01                                mov     r9b, 1
//.text : 00007FF62B5222F1 48 8B 56 60                             mov     rdx, [rsi + 60h]
//.text : 00007FF62B5222F5 48 8B 4E 50                             mov     rcx, [rsi + 50h]
//.text : 00007FF62B5222F9 E8 F2 78 64 00                          call    sub_7FF62BB69BF0
//.text : 00007FF62B5222FE 49 8B CE                                mov     rcx, r14
//.text : 00007FF62B522301 E8 6A 40 A1 FF                          call    sub_7FF62AF36370
//.text : 00007FF62B522306 4C 89 6C 24 68                          mov[rsp + 1E0h + var_178], r13
//.text : 00007FF62B52230B 44 89 6C 24 70                          mov     dword ptr[rsp + 1E0h + var_170], r13d
//.text : 00007FF62B522310 45 33 C0                                xor     r8d, r8d
//.text : 00007FF62B522313 48 8B D0                                mov     rdx, rax
//.text : 00007FF62B522316 48 8D 4C 24 68                          lea     rcx, [rsp + 1E0h + var_178]
//.text : 00007FF62B52231B E8 F0 7C 87 FF                          call    sub_7FF62AD9A010
//.text : 00007FF62B522320 90                                      nop
//.text : 00007FF62B522321
//.text : 00007FF62B522321                         loc_7FF62B522321 : ; DATA XREF : .rdata : 00007FF62C889C4C↓o
//.text : 00007FF62B522321 4C 89 6C 24 40                          mov[rsp + 1E0h + var_1A0], r13
//.text : 00007FF62B522326 44 89 6C 24 48                          mov[rsp + 1E0h + var_198], r13d
//.text : 00007FF62B52232B
//.text : 00007FF62B52232B                         loc_7FF62B52232B : ; DATA XREF : .rdata : 00007FF62C889C54↓o
//.text : 00007FF62B52232B 4C 8D 05 BE 5F CB 00                    lea     r8, Str1
//.text : 00007FF62B522332 4C 8B 74 24 68                          mov     r14, [rsp + 1E0h + var_178]
//.text : 00007FF62B522337 4D 85 F6                                test    r14, r14
//.text : 00007FF62B52233A 4D 0F 45 C6                             cmovnz  r8, r14
//.text : 00007FF62B52233E 48 8D 54 24 40                          lea     rdx, [rsp + 1E0h + var_1A0]
//.text : 00007FF62B522343 48 8B 4E 08                             mov     rcx, [rsi + 8]
//.text:00007FF62B522347 E8 D4 AE 64 00                          call    sub_7FF62BB6D220
//.text : 00007FF62B52234C 8B 44 24 48                             mov     eax, [rsp + 1E0h + var_198]
//.text : 00007FF62B522350 C1 E8 06                                shr     eax, 6
//.text : 00007FF62B522353 A8 01                                   test    al, 1
//.text : 00007FF62B522355 74 0A                                   jz      short loc_7FF62B522361
//.text : 00007FF62B522357 48 8B 44 24 50                          mov     rax, [rsp + 1E0h + var_190]
//.text : 00007FF62B52235C 4C 8B 00                                mov     r8, [rax]
//.text : 00007FF62B52235F EB 05                                   jmp     short loc_7FF62B522366
//.text : 00007FF62B522361; -------------------------------------------------------------------------- -
//.text:00007FF62B522361
//.text : 00007FF62B522361                         loc_7FF62B522361 : ; CODE XREF : EnemyHealth__Update + 2B5↑j
//.text : 00007FF62B522361 4C 8B 44 24 50                          mov     r8, [rsp + 1E0h + var_190]
//.text : 00007FF62B522366
//.text : 00007FF62B522366                         loc_7FF62B522366 : ; CODE XREF : EnemyHealth__Update + 2BF↑j
//.text : 00007FF62B522366 45 33 C9                                xor     r9d, r9d
//.text : 00007FF62B522369 48 8B 56 78                             mov     rdx, [rsi + 78h]
//.text : 00007FF62B52236D 48 8B 4E 68                             mov     rcx, [rsi + 68h]
//.text : 00007FF62B522371 E8 8A A0 64 00                          call    sub_7FF62BB6C400
//.text : 00007FF62B522376 8B 44 24 48                             mov     eax, [rsp + 1E0h + var_198]
//.text : 00007FF62B52237A C1 E8 06                                shr     eax, 6
//.text : 00007FF62B52237D A8 01                                   test    al, 1
//.text : 00007FF62B52237F 74 19                                   jz      short loc_7FF62B52239A
//.text : 00007FF62B522381 4C 8B 44 24 50                          mov     r8, [rsp + 1E0h + var_190]
//.text : 00007FF62B522386 48 8D 54 24 40                          lea     rdx, [rsp + 1E0h + var_1A0]
//.text : 00007FF62B52238B 48 8B 4C 24 40                          mov     rcx, [rsp + 1E0h + var_1A0]
//.text : 00007FF62B522390 E8 5B 84 64 00                          call    sub_7FF62BB6A7F0
//.text : 00007FF62B522395 4C 89 6C 24 40                          mov[rsp + 1E0h + var_1A0], r13
//.text : 00007FF62B52239A
//.text : 00007FF62B52239A                         loc_7FF62B52239A : ; CODE XREF : EnemyHealth__Update + 2DF↑j
//.text : 00007FF62B52239A 44 89 6C 24 48                          mov[rsp + 1E0h + var_198], r13d
//.text : 00007FF62B52239F
//.text : 00007FF62B52239F                         loc_7FF62B52239F : ; DATA XREF : .rdata : 00007FF62C889C5C↓o
//.text : 00007FF62B52239F 4D 85 F6                                test    r14, r14
//.text : 00007FF62B5223A2 74 2F                                   jz      short loc_7FF62B5223D3
//.text : 00007FF62B5223A4
//.text : 00007FF62B5223A4                         loc_7FF62B5223A4 : ; DATA XREF : .rdata : 00007FF62C889C64↓o
//.text : 00007FF62B5223A4 83 3D DD 12 66 01 02                    cmp     cs : dword_7FF62CB83688, 2
//.text : 00007FF62B5223AB 74 13                                   jz      short loc_7FF62B5223C0
//.text : 00007FF62B5223AD 48 8D 15 D4 12 66 01                    lea     rdx, dword_7FF62CB83688
//.text : 00007FF62B5223B4 48 8D 0D 45 0E 66 01                    lea     rcx, unk_7FF62CB83200
//.text : 00007FF62B5223BB E8 50 FE 37 00                          call    sub_7FF62B8A2210
//.text : 00007FF62B5223C0
//.text : 00007FF62B5223C0                         loc_7FF62B5223C0 : ; CODE XREF : EnemyHealth__Update + 30B↑j
//.text : 00007FF62B5223C0 45 33 C0                                xor     r8d, r8d
//.text : 00007FF62B5223C3 49 8B D6                                mov     rdx, r14
//.text : 00007FF62B5223C6 48 8D 0D 33 0E 66 01                    lea     rcx, unk_7FF62CB83200
//.text : 00007FF62B5223CD E8 CE FC 37 00                          call    sub_7FF62B8A20A0
//.text : 00007FF62B5223D2 90                                      nop
//.text : 00007FF62B5223D3
//.text : 00007FF62B5223D3                         loc_7FF62B5223D3 : ; CODE XREF : EnemyHealth__Update + 1B4↑j
//.text : 00007FF62B5223D3; EnemyHealth__Update + 1C1↑j ...
//.text:00007FF62B5223D3 C6 86 80 00 00 00 00                    mov     byte ptr[rsi + 80h], 0
//.text : 00007FF62B5223DA
//.text : 00007FF62B5223DA                         loc_7FF62B5223DA : ; DATA XREF : .rdata : 00007FF62C889C74↓o
//.text : 00007FF62B5223DA 4D 8B CC                                mov     r9, r12; void(__cdecl *)(void *)
//.text:00007FF62B5223DD BA 18 00 00 00                          mov     edx, 18h; unsigned __int64
//.text:00007FF62B5223E2 44 8D 42 E9                             lea     r8d, [rdx - 17h]; unsigned __int64
//.text:00007FF62B5223E6 48 8D 4C 24 78                          lea     rcx, [rsp + 1E0h + var_168]; void *
//.text:00007FF62B5223EB E8 0C F6 AD 00                          call ? ? _M@YAXPEAX_K1P6AX0@Z@Z; `eh vector destructor iterator'(void *,unsigned __int64,unsigned __int64,void (*)(void *))
//.text:00007FF62B5223F0 E9 D9 01 00 00                          jmp     loc_7FF62B5225CE
//.text : 00007FF62B5223F5; -------------------------------------------------------------------------- -
//.text:00007FF62B5223F5
//.text : 00007FF62B5223F5                         loc_7FF62B5223F5 : ; CODE XREF : EnemyHealth__Update + 129↑j
//.text : 00007FF62B5223F5 0F 2E 76 30                             ucomiss xmm6, dword ptr[rsi + 30h]
//.text : 00007FF62B5223F9 74 05                                   jz      short loc_7FF62B522400
//.text : 00007FF62B5223FB 41 B4 01                                mov     r12b, 1
//.text : 00007FF62B5223FE EB 03                                   jmp     short loc_7FF62B522403
//.text : 00007FF62B522400; -------------------------------------------------------------------------- -
//.text:00007FF62B522400
//.text : 00007FF62B522400                         loc_7FF62B522400 : ; CODE XREF : EnemyHealth__Update + 359↑j
//.text : 00007FF62B522400 45 32 E4                                xor     r12b, r12b
//.text : 00007FF62B522403
//.text : 00007FF62B522403                         loc_7FF62B522403 : ; CODE XREF : EnemyHealth__Update + 35E↑j
//.text : 00007FF62B522403 48 8B 15 5E AA 6C 02                    mov     rdx, cs : g_ThePlayer
//.text : 00007FF62B52240A 49 8B CE                                mov     rcx, r14
//.text : 00007FF62B52240D E8 FE 36 DA FF                          call    sub_7FF62B2C5B10
//.text : 00007FF62B522412 84 C0                                   test    al, al
//.text : 00007FF62B522414 0F 85 F1 00 00 00                       jnz     loc_7FF62B52250B
//.text : 00007FF62B52241A 45 84 E4                                test    r12b, r12b
//.text : 00007FF62B52241D 0F 85 F1 00 00 00                       jnz     loc_7FF62B522514
//.text : 00007FF62B522423 8B 05 E7 F4 70 02                       mov     eax, cs : dword_7FF62DC31910
//.text : 00007FF62B522429 2B 46 2C                                sub     eax, [rsi + 2Ch]
//.text : 00007FF62B52242C 3B 05 2E 6A 59 01                       cmp     eax, cs : dword_7FF62CAB8E60
//.text : 00007FF62B522432 0F 87 A9 00 00 00                       ja      loc_7FF62B5224E1
//.text : 00007FF62B522438 48 8B 05 29 AA 6C 02                    mov     rax, cs : g_ThePlayer
//.text : 00007FF62B52243F F3 41 0F 10 56 5C                       movss   xmm2, dword ptr[r14 + 5Ch]
//.text : 00007FF62B522445 F3 0F 5C 50 5C                          subss   xmm2, dword ptr[rax + 5Ch]
//.text : 00007FF62B52244A F3 41 0F 10 4E 58                       movss   xmm1, dword ptr[r14 + 58h]
//.text : 00007FF62B522450 F3 0F 5C 48 58                          subss   xmm1, dword ptr[rax + 58h]
//.text : 00007FF62B522455 F3 41 0F 10 46 54                       movss   xmm0, dword ptr[r14 + 54h]
//.text : 00007FF62B52245B F3 0F 5C 40 54                          subss   xmm0, dword ptr[rax + 54h]
//.text : 00007FF62B522460 F3 0F 11 44 24 58                       movss[rsp + 1E0h + var_188], xmm0
//.text : 00007FF62B522466 F3 0F 11 4C 24 5C                       movss[rsp + 1E0h + var_184], xmm1
//.text : 00007FF62B52246C F3 0F 11 54 24 60                       movss[rsp + 1E0h + var_180], xmm2
//.text : 00007FF62B522472 48 8D 4C 24 58                          lea     rcx, [rsp + 1E0h + var_188]
//.text : 00007FF62B522477 E8 34 F2 3C 00                          call    sub_7FF62B8F16B0
//.text : 00007FF62B52247C 0F 28 C8                                movaps  xmm1, xmm0
//.text : 00007FF62B52247F 48 8B 05 B2 94 66 02                    mov     rax, cs : qword_7FF62DB8B938
//.text : 00007FF62B522486 F3 0F 5C 88 54 01 00 00                 subss   xmm1, dword ptr[rax + 154h]
//.text : 00007FF62B52248E 0F 2F 0D 13 66 CC 00                    comiss  xmm1, cs : dword_7FF62C1E8AA8
//.text : 00007FF62B522495 77 0A                                   ja      short loc_7FF62B5224A1
//.text : 00007FF62B522497 F3 0F 58 0D ED 65 CC 00                 addss   xmm1, cs : Y
//.text : 00007FF62B52249F EB 18                                   jmp     short loc_7FF62B5224B9
//.text : 00007FF62B5224A1; -------------------------------------------------------------------------- -
//.text:00007FF62B5224A1
//.text : 00007FF62B5224A1                         loc_7FF62B5224A1 : ; CODE XREF : EnemyHealth__Update + 3F5↑j
//.text : 00007FF62B5224A1 0F 2F 0D E0 65 CC 00                    comiss  xmm1, cs : dword_7FF62C1E8A88
//.text : 00007FF62B5224A8 76 0F                                   jbe     short loc_7FF62B5224B9
//.text : 00007FF62B5224AA F3 0F 10 05 DA 65 CC 00                 movss   xmm0, cs : Y
//.text : 00007FF62B5224B2 F3 0F 5C C1                             subss   xmm0, xmm1
//.text : 00007FF62B5224B6 0F 28 C8                                movaps  xmm1, xmm0
//.text : 00007FF62B5224B9
//.text : 00007FF62B5224B9                         loc_7FF62B5224B9 : ; CODE XREF : EnemyHealth__Update + 3FF↑j
//.text : 00007FF62B5224B9; EnemyHealth__Update + 408↑j
//.text:00007FF62B5224B9 F3 0F 59 0D 27 D5 CF 00                 mulss   xmm1, cs : dword_7FF62C21F9E8
//.text : 00007FF62B5224C1 0F 54 0D E8 65 CC 00                    andps   xmm1, cs : xmmword_7FF62C1E8AB0
//.text : 00007FF62B5224C8 66 0F 6E 05 A8 69 59 01                 movd    xmm0, cs : dword_7FF62CAB8E78
//.text : 00007FF62B5224D0 0F 5B C0                                cvtdq2ps xmm0, xmm0
//.text : 00007FF62B5224D3 0F 2F C8                                comiss  xmm1, xmm0
//.text : 00007FF62B5224D6 0F 97 C0                                setnbe  al
//.text : 00007FF62B5224D9 84 C0                                   test    al, al
//.text : 00007FF62B5224DB 0F 84 ED 00 00 00                       jz      loc_7FF62B5225CE
//.text : 00007FF62B5224E1
//.text : 00007FF62B5224E1                         loc_7FF62B5224E1 : ; CODE XREF : EnemyHealth__Update + 392↑j
//.text : 00007FF62B5224E1 48 85 FF                                test    rdi, rdi
//.text : 00007FF62B5224E4 0F 85 E4 00 00 00                       jnz     loc_7FF62B5225CE
//.text : 00007FF62B5224EA 48 8B 05 77 A9 6C 02                    mov     rax, cs : g_ThePlayer
//.text : 00007FF62B5224F1 F6 80 DD 0B 00 00 04                    test    byte ptr[rax + 0BDDh], 4
//.text:00007FF62B5224F8 0F 85 D0 00 00 00                       jnz     loc_7FF62B5225CE
//.text : 00007FF62B5224FE 48 8B CE                                mov     rcx, rsi
//.text : 00007FF62B522501 E8 7A 03 00 00                          call    EnemyHealthSetPercent
//.text : 00007FF62B522506 E9 C3 00 00 00                          jmp     loc_7FF62B5225CE
//.text : 00007FF62B52250B; -------------------------------------------------------------------------- -
//.text:00007FF62B52250B
//.text : 00007FF62B52250B                         loc_7FF62B52250B : ; CODE XREF : EnemyHealth__Update + 374↑j
//.text : 00007FF62B52250B 45 84 E4                                test    r12b, r12b
//.text : 00007FF62B52250E 0F 84 BA 00 00 00                       jz      loc_7FF62B5225CE
//.text : 00007FF62B522514
//.text : 00007FF62B522514                         loc_7FF62B522514 : ; CODE XREF : EnemyHealth__Update + 37D↑j
//.text : 00007FF62B522514 8B 05 F6 F3 70 02                       mov     eax, cs : dword_7FF62DC31910
//.text : 00007FF62B52251A 89 46 2C                                mov[rsi + 2Ch], eax
//.text : 00007FF62B52251D F3 0F 11 76 30                          movss   dword ptr[rsi + 30h], xmm6
//.text : 00007FF62B522522 4C 8D 25 D7 9E C9 FF                    lea     r12, callbeforeenemy
//.text : 00007FF62B522529 4C 89 64 24 20                          mov[rsp + 1E0h + var_1C0], r12; void(__cdecl *)(void *)
//.text:00007FF62B52252E 4C 8D 0D BB 92 C9 FF                    lea     r9, sub_7FF62B1BB7F0; void(__cdecl *)(void *)
//.text:00007FF62B522535 BA 18 00 00 00                          mov     edx, 18h; unsigned __int64
//.text:00007FF62B52253A 44 8D 42 E9                             lea     r8d, [rdx - 17h]; unsigned __int64
//.text:00007FF62B52253E 48 8D 4D 90                             lea     rcx, [rbp + 118h + var_188]; void *
//.text:00007FF62B522542 E8 1D F6 AD 00                          call ? ? _L@YAXPEAX_K1P6AX0@Z2@Z; `eh vector constructor iterator'(void *,unsigned __int64,unsigned __int64,void (*)(void *),void (*)(void *))
//.text:00007FF62B522547 90                                      nop
//.text : 00007FF62B522548
//.text : 00007FF62B522548                         loc_7FF62B522548 : ; DATA XREF : .rdata : 00007FF62C889C7C↓o
//.text : 00007FF62B522548 0F 57 C9                                xorps   xmm1, xmm1
//.text : 00007FF62B52254B F3 0F 5A CE                             cvtss2sd xmm1, xmm6
//.text : 00007FF62B52254F 48 8D 4D 90                             lea     rcx, [rbp + 118h + var_188]
//.text : 00007FF62B522553 E8 E8 73 FD FF                          call    sub_7FF62B4F9940
//.text : 00007FF62B522558 8B 46 40                                mov     eax, [rsi + 40h]
//.text : 00007FF62B52255B 24 8F                                   and     al, 8Fh
//.text : 00007FF62B52255D 3C 08                                   cmp     al, 8
//.text:00007FF62B52255F 0F 94 C0                                setz    al
//.text : 00007FF62B522562 88 44 24 30                             mov[rsp + 1E0h + var_1B0], al
//.text : 00007FF62B522566 48 C7 44 24 28 01 00 00 + mov[rsp + 1E0h + var_1B8], 1
//.text : 00007FF62B52256F 48 8D 45 90                             lea     rax, [rbp + 118h + var_188]
//.text : 00007FF62B522573 48 89 44 24 20                          mov[rsp + 1E0h + var_1C0], rax
//.text : 00007FF62B522578 4C 8D 0D E1 9A E4 00                    lea     r9, aSettargetperce; "SetTargetPercent"
//.text:00007FF62B52257F 45 33 C0                                xor     r8d, r8d
//.text : 00007FF62B522582 48 8B 56 48                             mov     rdx, [rsi + 48h]
//.text : 00007FF62B522586 48 8B 4E 38                             mov     rcx, [rsi + 38h]
//.text : 00007FF62B52258A E8 E1 79 64 00                          call    sub_7FF62BB69F70
//.text : 00007FF62B52258F 90                                      nop
//.text : 00007FF62B522590
//.text : 00007FF62B522590                         loc_7FF62B522590 : ; DATA XREF : .rdata : 00007FF62C889C84↓o
//.text : 00007FF62B522590 4D 8B CC                                mov     r9, r12; void(__cdecl *)(void *)
//.text:00007FF62B522593 BA 18 00 00 00                          mov     edx, 18h; unsigned __int64
//.text:00007FF62B522598 44 8D 42 E9                             lea     r8d, [rdx - 17h]; unsigned __int64
//.text:00007FF62B52259C 48 8D 4D 90                             lea     rcx, [rbp + 118h + var_188]; void *
//.text:00007FF62B5225A0 E8 57 F4 AD 00                          call ? ? _M@YAXPEAX_K1P6AX0@Z@Z; `eh vector destructor iterator'(void *,unsigned __int64,unsigned __int64,void (*)(void *))
//.text:00007FF62B5225A5 EB 27                                   jmp     short loc_7FF62B5225CE
//.text : 00007FF62B5225A7; -------------------------------------------------------------------------- -
//.text:00007FF62B5225A7
//.text : 00007FF62B5225A7                         loc_7FF62B5225A7 : ; CODE XREF : EnemyHealth__Update + A7↑j
//.text : 00007FF62B5225A7; EnemyHealth__Update + B4↑j ...
//.text:00007FF62B5225A7 48 8B 0D BA A8 6C 02                    mov     rcx, cs : g_ThePlayer
//.text : 00007FF62B5225AE 48 85 FF                                test    rdi, rdi
//.text : 00007FF62B5225B1 75 13                                   jnz     short loc_7FF62B5225C6
//.text : 00007FF62B5225B3 F6 81 DD 0B 00 00 04                    test    byte ptr[rcx + 0BDDh], 4
//.text:00007FF62B5225BA 75 0A                                   jnz     short loc_7FF62B5225C6
//.text : 00007FF62B5225BC 48 8B CE                                mov     rcx, rsi
//.text : 00007FF62B5225BF E8 BC 02 00 00                          call    EnemyHealthSetPercent
//.text : 00007FF62B5225C4 EB 08                                   jmp     short loc_7FF62B5225CE
//.text:00007FF62B5225C6; -------------------------------------------------------------------------- -
//.text:00007FF62B5225C6
//.text : 00007FF62B5225C6                         loc_7FF62B5225C6 : ; CODE XREF : EnemyHealth__Update + 511↑j
//.text : 00007FF62B5225C6; EnemyHealth__Update + 51A↑j
//.text:00007FF62B5225C6 B2 01                                   mov     dl, 1
//.text : 00007FF62B5225C8 E8 83 31 E3 FF                          call    sub_7FF62B355750
//.text : 00007FF62B5225CD 90                                      nop
//.text : 00007FF62B5225CE
//.text : 00007FF62B5225CE                         loc_7FF62B5225CE : ; CODE XREF : EnemyHealth__Update + 350↑j
//.text : 00007FF62B5225CE; EnemyHealth__Update + 43B↑j ...
//.text:00007FF62B5225CE 48 85 FF                                test    rdi, rdi
//.text : 00007FF62B5225D1 74 22                                   jz      short loc_7FF62B5225F5
//.text : 00007FF62B5225D3 48 8D 4F 20                             lea     rcx, [rdi + 20h]
//.text : 00007FF62B5225D7 41 8B C7                                mov     eax, r15d
//.text : 00007FF62B5225DA F0 0F C1 41 08                          lock xadd[rcx + 8], eax
//.text:00007FF62B5225DF FF C8                                   dec     eax
//.text : 00007FF62B5225E1 A9 FF 03 00 00                          test    eax, 3FFh
//.text : 00007FF62B5225E6 75 06                                   jnz     short loc_7FF62B5225EE
//.text : 00007FF62B5225E8 48 8B 01                                mov     rax, [rcx]
//.text : 00007FF62B5225EB FF 50 08                                call    qword ptr[rax + 8]
//.text:00007FF62B5225EE
//.text : 00007FF62B5225EE                         loc_7FF62B5225EE : ; CODE XREF : EnemyHealth__Update + 546↑j
//.text : 00007FF62B5225EE 48 8B 9D F8 00 00 00                    mov     rbx, [rbp + 118h + TESObjectREFR_ptr]
//.text : 00007FF62B5225F5
//.text : 00007FF62B5225F5                         loc_7FF62B5225F5 : ; CODE XREF : EnemyHealth__Update + 531↑j
//.text : 00007FF62B5225F5; DATA XREF : .rdata : 00007FF62C889C94↓o
//.text : 00007FF62B5225F5 48 85 DB                                test    rbx, rbx
//.text : 00007FF62B5225F8 74 1D                                   jz      short loc_7FF62B522617
//.text : 00007FF62B5225FA 48 8D 4B 20                             lea     rcx, [rbx + 20h]
//.text : 00007FF62B5225FE F0 44 0F C1 79 08                       lock xadd[rcx + 8], r15d
//.text:00007FF62B522604 41 FF CF                                dec     r15d
//.text : 00007FF62B522607 41 F7 C7 FF 03 00 00                    test    r15d, 3FFh
//.text : 00007FF62B52260E 75 07                                   jnz     short loc_7FF62B522617
//.text : 00007FF62B522610 48 8B 01                                mov     rax, [rcx]
//.text : 00007FF62B522613 FF 50 08                                call    qword ptr[rax + 8]
//.text:00007FF62B522616 90                                      nop
//.text : 00007FF62B522617
//.text : 00007FF62B522617                         loc_7FF62B522617 : ; CODE XREF : EnemyHealth__Update + 558↑j
//.text : 00007FF62B522617; EnemyHealth__Update + 56E↑j
//.text:00007FF62B522617; DATA XREF : ...
//.text : 00007FF62B522617 48 8B 9C 24 08 02 00 00                 mov     rbx, [rsp + 1E0h + arg_18]
//.text:00007FF62B52261F 0F 28 B4 24 A0 01 00 00                 movaps  xmm6, [rsp + 1E0h + var_40]
//.text : 00007FF62B522627 48 81 C4 B0 01 00 00                    add     rsp, 1B0h
//.text : 00007FF62B52262E 41 5F                                   pop     r15
//.text : 00007FF62B522630 41 5E                                   pop     r14
//.text : 00007FF62B522632 41 5D                                   pop     r13
//.text : 00007FF62B522634 41 5C                                   pop     r12
//.text : 00007FF62B522636 5F                                      pop     rdi
//.text : 00007FF62B522637 5E                                      pop     rsi
//.text : 00007FF62B522638 5D                                      pop     rbp
//.text : 00007FF62B522639 C3                                      retn
//.text : 00007FF62B522639                         EnemyHealth__Update endp
//.text : 00007FF62B522639
//.text : 00007FF62B522639; -------------------------------------------------------------------------- -

RelocAddr<uintptr_t> Enemy_Update_Hook_Target(Enemy_Update_Hook_Base + 0x44);
void EnemyHealth_Update_Hook(UInt32 * refHandle, TESObjectREFR ** refrOut)
{
   TESObjectREFR * reference = (*refrOut);
   if (!reference)
   {
      return;
   }
   UInt16 npcLevel = 0;

   if (reference)
   {
      if (reference->baseForm->formType == kFormType_NPC ||
         reference->baseForm->formType == kFormType_Character)
      {
         Actor * pNPC = DYNAMIC_CAST(reference, TESObjectREFR, Actor);
         if (pNPC)
         {
            npcLevel = CALL_MEMBER_FN(pNPC, GetLevel)();
         }
      }
   }

   ahzTargetHandle.Lock();
   ahzTargetHandle.m_data = npcLevel;
   ahzTargetHandle.Release();
}
//RelocPtr<SimpleLock>		globalMenuStackLock(0x1EE4A60);
UInt16 GetCurrentEnemyLevel()
{
   UInt16 refr = 0;
   ahzTargetHandle.Lock();
   refr = ahzTargetHandle.m_data;
   ahzTargetHandle.Release();
   return refr;
}

void AHZInstallEnemyHealthUpdateHook()
{
   struct HUDMenu_RegisterExtension_Code : Xbyak::CodeGenerator {
      HUDMenu_RegisterExtension_Code(void * buf, UInt64 funcAddr) : Xbyak::CodeGenerator(4096, buf)
      {
         Xbyak::Label retnLabel;
         Xbyak::Label funcLabel;

         // Call original code and use the same signature as LookupREFRByHandle to pass the object ref to the hooked
         // function
         call(LookupREFRByHandle);

         // Call our function (Same signature as LookupREFRByHandle)
         call(ptr[rip + funcLabel]);

         // Jump back from the Trampoline
         jmp(ptr[rip + retnLabel]);

         L(funcLabel);
         dq(funcAddr);

         L(retnLabel);
         dq(Enemy_Update_Hook_Target.GetUIntPtr() + 5);
      }
   };

   void * codeBuf = g_localTrampoline.StartAlloc();
   HUDMenu_RegisterExtension_Code code(codeBuf, (uintptr_t)EnemyHealth_Update_Hook);
   g_localTrampoline.EndAlloc(code.getCurr());

   g_branchTrampoline.Write5Branch(Enemy_Update_Hook_Target.GetUIntPtr(), uintptr_t(code.getCode()));
}