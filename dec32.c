#include <stdio.h>
#include <stdint.h>

/* Simplistic adler32 replacement */
uint32_t dec32(const uint8_t * message, size_t length) {
	size_t A = 1;
	size_t B = 0;
	size_t count;
/* We establish pointer size at runtime */
	uint8_t ptr = sizeof(size_t);
	for(count = 0; count < length; ++count) {
		A += message[count];
		B += A;
/* If we added enough bytes that fit in a register,
 * and count is non-trivial, we modulo our checksum components. */
		if(!(count%ptr) && count) {
			A %= 65521;
			B %= 66521; } }
/* At the end of the digesting process, we need to modulo our
 * components just in case so they can fit into our return. */
	if(count%ptr) {
	A %= 65521;
	B %= 65521; }
	return (B << 16) | A;
}
