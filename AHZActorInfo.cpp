#include "AHZActorInfo.h"

// Is Actor Sentient = 606850
// Get Soul Level = 3C1740


typedef  UInt32 (__fastcall *GET_ACTOR_IS_SENTIENT)(Actor *theActor);
RelocAddr<GET_ACTOR_IS_SENTIENT>pGetActorIsSentient(0x00606850);
//.text:0000000140606850; == == == == == == == = S U B R O U T I N E == == == == == == == == == == == == == == == == == == == =
//.text:0000000140606850
//.text:0000000140606850
//.text:0000000140606850                         IsActorSentient proc near; CODE XREF : sub_1401E5050 + 8E↑p
//.text : 0000000140606850; sub_140548110 + 80↑p ...
//.text:0000000140606850 48 8B 81 F0 00 00 00                    mov     rax, [rcx + 0F0h]
//.text:0000000140606857 48 85 C0                                test    rax, rax
//.text:000000014060685A 74 27                                   jz      short loc_140606883
//.text:000000014060685C 48 83 78 50 00                          cmp     qword ptr[rax + 50h], 0
//.text:0000000140606861 74 20                                   jz      short loc_140606883
//.text:0000000140606863 48 8B 50 50                             mov     rdx, [rax + 50h]
//.text:0000000140606867 48 85 D2                                test    rdx, rdx
//.text:000000014060686A 75 03                                   jnz     short loc_14060686F
//.text:000000014060686C 32 C0                                   xor     al, al
//.text:000000014060686E C3                                      retn
//.text : 000000014060686F; -------------------------------------------------------------------------- -
//.text:000000014060686F
//.text:000000014060686F                         loc_14060686F : ; CODE XREF : IsActorSentient + 1A↑j
//.text : 000000014060686F 8B 42 2C                                mov     eax, [rdx + 2Ch]
//.text:0000000140606872 C1 E8 19                                shr     eax, 19h
//.text:0000000140606875 F6 D0                                   not al
//.text:0000000140606877 A8 01                                   test    al, 1
//.text:0000000140606879 75 08                                   jnz     short loc_140606883
//.text:000000014060687B 8B 42 28                                mov     eax, [rdx + 28h]
//.text:000000014060687E D1 E8                                   shr     eax, 1
//.text : 0000000140606880 24 01 and al, 1
//.text:0000000140606882 C3                                      retn
//.text:0000000140606883; -------------------------------------------------------------------------- -
//.text:0000000140606883
//.text:0000000140606883                         loc_140606883 : ; CODE XREF : IsActorSentient + A↑j
//.text : 0000000140606883; IsActorSentient + 11↑j ...
//.text:0000000140606883 48 8B 01                                mov     rax, [rcx]
//.text:0000000140606886 48 FF A0 B0 08 00 00                    jmp     qword ptr[rax + 8B0h]
//.text:0000000140606886                         IsActorSentient endp
//.text:0000000140606886
//.text:0000000140606886; -------------------------------------------------------------------------- -


typedef  UInt32 (__cdecl *GET_ACTOR_SOUL_TYPE)(UInt32 level, UInt8 isSetient);
RelocAddr<GET_ACTOR_SOUL_TYPE>pGetActorSoulType(0x003C1740);
//.text:00000001403C1740; == == == == == == == = S U B R O U T I N E == == == == == == == == == == == == == == == == == == == =
//.text:00000001403C1740
//.text : 00000001403C1740
//.text : 00000001403C1740; signed __int64 __fastcall GetSoulLevel(unsigned int level, char IsSentient)
//.text:00000001403C1740                         GetSoulLevel    proc near; CODE XREF : sub_140634830 + 3C↓p
//.text : 00000001403C1740; sub_1406348A0 + 46↓j ...
//.text:00000001403C1740 84 D2                                   test    dl, dl
//.text : 00000001403C1742 75 30                                   jnz     short loc_1403C1774
//.text : 00000001403C1744 3B 0D DE 3C A2 01                       cmp     ecx, cs : dword_141DE5428
//.text : 00000001403C174A 73 28                                   jnb     short loc_1403C1774
//.text : 00000001403C174C 3B 0D BE 3C A2 01                       cmp     ecx, cs : dword_141DE5410
//.text : 00000001403C1752 72 06                                   jb      short loc_1403C175A
//.text : 00000001403C1754 B8 04 00 00 00                          mov     eax, 4
//.text : 00000001403C1759 C3                                      retn
//.text : 00000001403C175A; -------------------------------------------------------------------------- -


UInt32 CAHZActorInfo::IsSentient(Actor *actor)
{
	return pGetActorIsSentient(actor);
}

UInt32 CAHZActorInfo::GetSoulType(UInt16 actorLevel, UInt8 isActorSentient)
{
	UInt32 type = pGetActorSoulType(actorLevel, isActorSentient);
	return type;
}
