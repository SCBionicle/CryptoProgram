// EncryptData.cpp
//
// This file uses the input data and key information to encrypt the input data
//

#include "Main.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// code to encrypt the data as specified by the project assignment
int encryptData(char *data, int dataLength)
{
	int resulti = 0;
	gdebug1 = 0;					// a couple of global variables that could be used for debugging
	gdebug2 = 0;					// also can have a breakpoint in C code

	int index= gNumRounds;
	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers
	
	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds
		
		// simple example that xors 2nd byte of data with 14th byte in the key file
		xor ecx, ecx //clear ecx from any residual data from prior operations 
		xor ebx, ebx
		xor eax, eax
		mov edi, data				// Put ADDRESS of first data element into edi,
	Round:
		lea esi, gPasswordHash
		mov ah, [esi]
		mov al, [esi+1]
		mov bh, [esi+2]
		mov bl, [esi+3]
		lea esi, gkey
		test bx, bx
		jne Start
		mov bx, 0xFFFF
		mov dl, byte ptr[edi + ecx]
		xor dl, byte ptr[esi + eax]
		mov byte ptr[edi + ecx], dl
		Start: // start of the loop 
////////////////////////////////////////////////////////////////////////////////////////
		//xor byte ptr[edi + ecx], 'A' //Seth		// Exclusive-or byte
			mov dl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			rol dl, 1
			mov byte ptr[edi + ecx], dl
			//xor byte ptr[edi + ecx], 'E'MAX
			mov dl, byte ptr[edi+ecx]
			lea  esi, gEncodeTable
			movzx edx, dl
			mov dl, byte ptr[esi+edx]
			mov byte ptr[edi+ecx], dl
			//xor byte ptr[edi + ecx], 'B' //Seth 
			mov dl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov dh, dl
			shr dh, 4 //upper->lower
			shl dl, 4//lower->upper
			add dl, bh //add lower to upper
			mov byte ptr[edi + ecx], dl //move back to memory
	
////////////////////////////////////////////////////////////////////////////////////
			//xor byte ptr[edi + ecx], 'D'
			mov dl, byte ptr[edi + ecx]
			mov dh, dl
			and dh, 0x0F//higher bits cleared
			and dl, 0xF0 //lower bits cleared
			rol dl, 1
			jc CARRYSET
			jnc CONTINUE
		CARRYSET:
			or dl, 0x10
			xor dl,0x01
			jmp CONTINUE
		CONTINUE:
			ror dh, 1
			jc CARRYBL
			jnc LAST
		CARRYBL:
			or dl, 0x08
			xor dl,0x80
			jmp LAST
		LAST: 
			add dh, dl
			mov byte ptr[edi + ecx], dh

		//xor byte ptr[edi + ecx], 'C' 
		mov dl, byte ptr[edi + ecx]
		push eax 
		xor eax, eax 
		xor dh, dh  // set bl to 0x00
			CLOOP :
				rcr dl, 1
				rcl dh, 1
				inc eax
				cmp eax, 8
				jne CLOOP // Do 8 times
			pop eax
			mov	byte ptr[edi + ecx], dh
			jmp Check
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
} // encryptData
//aebdc - Ortiz
//