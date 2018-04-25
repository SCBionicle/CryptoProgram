// DecryptData.cpp
//
// THis file uses the input data and key information to decrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to decrypt the data as specified by the project assignment
int decryptData(char *data, int dataLength)
{
	int resulti = 0;
	int index = 0;
	int iNumOfRounds = gNumRounds;
	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds
		xor ecx, ecx //clear ecx from any residual data from prior operations 
		mov edi, data				// Put ADDRESS of first data element into edi,
	
		SetIndex :
			xor edx, edx
			xor ebx, ebx
			xor eax, eax
			lea esi, gPasswordHash
			mov edx, iNumOfRounds
			//starting index
			mov ah, [esi + edx * 4]
			mov al, [esi + edx * 4 + 1]
			mov [esp-8],eax
			//hop count 
			mov bh, [esi + edx * 4 + 2]
			mov bl, [esi + edx * 4 + 3]
			mov [esp-4], ebx
			cmp bx, 0
			jne Start
			mov bx, 0xFFFF
			mov[esp - 4], ebx
		Start : // start of the loop  
		//xor byte ptr[edi + ecx], 'C' 
		mov al, byte ptr[edi + ecx]
		xor dl, dl  // set bl to 0x00
		xor bl, bl
		CLOOP :
			rcr al, 1
			rcl dl, 1
			inc bl
			cmp bl, 8
			jne CLOOP // Do 8 times
			mov	byte ptr[edi + ecx], dl
		//xor byte ptr[edi + ecx], 'D'		//Eddie
			mov al, byte ptr[edi + ecx] // grab byte from data
			mov bl, al 
			mov dl, al
			and dl, 0xF0 //clear lower nibble
			and bl, 0x0F//clear upper nibble
			rol bl, 4 // shift to lower nibble
			ror dl, 4// shift to higher nibble
			rol bl, 1// left shift xxx0-000x
			jc CARRYSET //  if there is a one proceed to function
			jnc CONTINUE// else continue to next operation

		CARRYSET ://if cf has a one
			or bl, 0x10 //set 5th bit to 1,  1xxx-0001
			xor bl, 0x01//clear lower nibble, 1xxx-0000
			jmp CONTINUE//continue with program
		CONTINUE :
			ror dl, 1 // shift right x000-0xxx
			jc CARRYDL// if cf is set 
			jnc LAST//else finish step 

		CARRYDL :
			or dl, 0x08// set 4th bit to 1 1000-1xxx
			xor dl, 0x80// clear upper nibble 0000-1xxx
			jmp LAST
		LAST :
			rol dl, 4 // shift nibbles back to upper half
			ror bl, 4// shift nibbles back to lower half
			add bl, dl// put back together and added it back to the data
			mov byte ptr[edi + ecx], bl
		//xor byte ptr[edi + ecx], 'B' - Seth /
			mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov bh, bl
			shr bh, 4 //upper->lower
			shl bl, 4//lower->upper
			add bl, bh //add lower to upper
			mov byte ptr[edi + ecx], bl //move back to memory
		//xor byte ptr[edi + ecx], 'E' MAX
			mov dl, byte ptr[edi + ecx]
			lea esi, gDecodeTable
			movzx edx, dl
			mov  dl, byte ptr[esi + edx]
			mov byte ptr[edi + ecx], dl
			//////////////////////////////////////////////////////////////////////////////////////////////////	
		//xor byte ptr[edi + ecx], 'A'		// Seth - Exclusive-or byte
			mov bl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			ror bl, 1
			mov byte ptr[edi + ecx], bl	
		Hop:
			mov eax, [esp - 8]//load index 
			mov ebx, [esp - 4]//load hop count
		Hopping:
			xor edx, edx
			lea esi, gkey//load global key into esi
			mov dl, byte ptr[edi + ecx]
			xor dl, byte ptr[esi + eax]
			mov byte ptr[edi + ecx], dl
			add eax, ebx //add hop count to starting point
			cmp eax, 65537//cmp to max 
			jb Increment// if below 
			sub eax, 65537 // if greater subtract by this num
		Increment:
			inc ecx
			cmp ecx, dataLength // check to see if we have reached the end of the data file 
			jb Start // jump to start of loop if ecx is smaller than datalength 
		Rounds://check to see if there are more rounds to go 
			dec iNumOfRounds
			cmp iNumOfRounds, 0
			jne SetIndex
			}
	return resulti;
} // decryptData

