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

	int index=0;
	// You can not declare any local variables in C, but should use resulti to indicate any errors
	// Set up the stack frame and assign variables in assembly if you need to do so
	// access the parameters BEFORE setting up your own stack frame
	// Also, you cannot use a lot of global variables - work with registers
	
	__asm {// you will need to reference some of these global variables
		// (gptrPasswordHash or gPasswordHash), (gptrKey or gkey), gNumRounds
		
		// simple example that xors 2nd byte of data with 14th byte in the key file
		xor ecx, ecx //clear ecx from any residual data from prior operations 
		mov edi, data				// Put ADDRESS of first data element into edi,
		Start: // start of the loop 
////////////////////////////////////////////////////////////////////////////////////////
		//xor byte ptr[edi + ecx], 'A' //Seth		// Exclusive-or byte
			mov bl, byte ptr[edi + ecx] //move data byte to bl (part of ebx) to rotate
			rol bl, 1
			mov byte ptr[edi + ecx], bl
			//xor byte ptr[edi + ecx], 'E'MAX
			mov dl, byte ptr[edi+ecx]
			lea  esi, gEncodeTable
			movzx edx, dl
			mov dl, byte ptr[esi+edx]
			mov byte ptr[edi+ecx], dl
			//xor byte ptr[edi + ecx], 'B' //Seth 
			mov bl, byte ptr[edi + ecx] //bh = upper nibble, bl = lower nibble
			mov bh, bl
			shr bh, 4 //upper->lower
			shl bl, 4//lower->upper
			add bl, bh //add lower to upper
			mov byte ptr[edi+ecx], bl //move back to memory
	
////////////////////////////////////////////////////////////////////////////////////
			//xor byte ptr[edi + ecx], 'D'
			mov al, byte ptr[edi + ecx]
			mov bl, al
			mov dl, al
			and bl, 0x0F
			and dl, 0xF0
			rol dl, 1
			jc CARRYSET
			jnc CONTINUE
		CARRYSET:
			or dl, 0x10
			xor dl,0x01
			jmp CONTINUE
		CONTINUE:
			ror bl, 1
			jc CARRYBL
			jnc LAST
		CARRYBL:
			or bl, 0x08
			xor bl,0x80
			jmp LAST
		LAST: 
			add bl, dl
			mov byte ptr[edi + ecx], bl

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
			
			inc ecx
			cmp ecx, dataLength // check to see if we have reached the end of the data file 
			jb Start // jump to start of loop if ecx is smaller than datalength
	}
	return resulti;
} // encryptData