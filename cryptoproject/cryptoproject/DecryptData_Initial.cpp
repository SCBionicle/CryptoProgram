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
	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds

		xor ecx, ecx //clear ecx from any residual data from prior operations 
			xor ebx, ebx
			xor eax, eax
			mov edi, data				// Put ADDRESS of first data element into edi,
		Round :
		lea esi, gPasswordHash
			mov ah, [esi]
			mov al, [esi + 1]
			mov bh, [esi + 2]
			mov bl, [esi + 3]
			lea esi, gkey
			test bx, bx
			jne Start
			mov bx, 0xFFFF
			mov dl, byte ptr[edi+ecx]
			xor dl, byte ptr[esi + eax]
			mov byte ptr[edi+ecx],dl
	Start : // start of the loop  
//xor byte ptr[edi + ecx], 'C' 
		mov dl, byte ptr[edi + ecx]
		xor dh, dh  // set bl to 0x00
		push eax
		xor eax, eax 
		CLOOP :
			rcr dl, 1
			rcl dh, 1
			inc eax
			cmp eax, 8
			jne CLOOP // Do 8 times
			pop eax
			mov	byte ptr[edi + ecx], dh


//xor byte ptr[edi + ecx], 'D'		//Eddie
			  mov dl, byte ptr[edi + ecx]
			  mov dh, dl
			  and dl, 0xF0
			  and dh, 0x0F
			  rol dh, 4
			  ror dl, 4
			  rol dh, 1// left shift 
			  jc CARRYSET //  if there is a one proceed to function
			  jnc CONTINUE// else continue to next operation

		  CARRYSET ://if cf has a one
			or dh, 0x10 //set 5th bit to 1
			xor dh, 0x01//clear lower nibble
			jmp CONTINUE//continue with program
		CONTINUE :
		ror dl, 1 // shift right 
			jc CARRYDL// if cf is set 
			jnc LAST//else finish step 

		CARRYDL :
		or dl, 0x08// set 4th bit to 1
			xor dl, 0x80// clear upper nibble 
			jmp LAST
		LAST :
		rol dl, 4 // shift nibbles to upper half
			ror dh, 4// shift nibbles to lower half
			add dh, dl// put back together and added it back to the data
			mov byte ptr[edi + ecx], dh
//xor byte ptr[edi + ecx], 'B' - Seth /
			mov dl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov dh, dl
			shr dh, 4 //upper->lower
			shl dl, 4//lower->upper
			add dl, dh //add lower to upper
			mov byte ptr[edi+ecx], dl //move back to memory
//xor byte ptr[edi + ecx], 'E' MAX
			mov dl, byte ptr[edi+ecx] 
			lea esi,  gDecodeTable
			movzx edx, dl
			mov  dl, byte ptr[esi + edx]
			mov byte ptr[edi+ecx], dl
//////////////////////////////////////////////////////////////////////////////////////////////////	
		//xor byte ptr[edi + ecx], 'A'		// Seth - Exclusive-or byte
			mov dl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			ror dl, 1
			mov byte ptr[edi + ecx], dl
			jmp Check
/////////////////////////////////////////////////////////////////////////////////////////////////////	
		Check:
			dec index 
			cmp index, 0
			jne Round
			je Done
		Done:
				inc ecx
				cmp ecx, dataLength // check to see if we have reached the end of the data file 
				jb Start // jump to start of loop if ecx is smaller than datalength 
	
	}
	return resulti;
} // decryptData

