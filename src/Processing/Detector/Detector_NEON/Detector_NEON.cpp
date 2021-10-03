#include "Detector_NEON.hpp"

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

Detector_NEON::Detector_NEON() : Detector(1) {
}

void Detector_NEON::execute(float * buffer) {
#if 0
	float ref_ps = (*buffer) + *(buffer + 1) + *(buffer + 3) + *(buffer + 4) + *(buffer + 14) + *(buffer + 15) + *(buffer + 18) + *(buffer + 19);

	float ref_sum = 0.0;
	for (int j=0; j<32; j++){
		float temp = *(buffer + j);
		ref_sum += temp * temp;	
	}
	ref_sum = ref_sum * 8;

	printf("%f - %f\n", ref_ps, ref_sum);
#endif
#if defined(__ARM_NEON)
    const float32x2_t a0 = vld1_f32(buffer + 0);
    const float32x2_t a1 = vld1_f32(buffer + 3);
    const float32x2_t a2 = vld1_f32(buffer + 14);
    const float32x2_t a3 = vld1_f32(buffer + 18);

    const float32x2_t b0 = vadd_f32(a0, a1);
    const float32x2_t b1 = vadd_f32(a2, a3);
    const float32x2_t c0 = vadd_f32(b0, b1);

    const float ps = vget_lane_f32(vpadd_f32(c0, c0), 0);

    const float32x4_t r0 = vld1q_f32(buffer + 0);
    const float32x4_t r1 = vld1q_f32(buffer + 4);
    const float32x4_t r2 = vld1q_f32(buffer + 8);
    const float32x4_t r3 = vld1q_f32(buffer + 12);
    const float32x4_t r4 = vld1q_f32(buffer + 16);
    const float32x4_t r5 = vld1q_f32(buffer + 20);
    const float32x4_t r6 = vld1q_f32(buffer + 24);
    const float32x4_t r7 = vld1q_f32(buffer + 28);

    const float32x4_t s0 = vaddq_f32(vaddq_f32(vmulq_f32(r0, r0), vmulq_f32(r1, r1)), vaddq_f32(vmulq_f32(r2, r2), vmulq_f32(r3, r3)));
    const float32x4_t s1 = vaddq_f32(vaddq_f32(vmulq_f32(r4, r4), vmulq_f32(r5, r5)), vaddq_f32(vmulq_f32(r6, r6), vmulq_f32(r7, r7)));

    const float32x4_t t0 = vaddq_f32(s0, s1);

    const float32x2_t r   = vadd_f32(vget_high_f32(t0), vget_low_f32(t0));
    const float       sum = 8.0f * vget_lane_f32(vpadd_f32(r, r), 0);
#if 0
	printf("%f - %f\n", ps, sum);
    exit( EXIT_FAILURE );
#endif
	array[0] = (ps/sqrt(sum));
#else
    *buffer = 0; // No unused error
    printf("Le decodeur instancie pour realiser le traitement est le Detector_NEON, mais ce dernier");
    printf("ne peut pas fonctionner sur le systeme hote...");
    exit(EXIT_FAILURE);
#endif
}

void Detector_NEON::execute(std::vector<float> * iBuffer, std::vector<float> * oBuffer) {
    const uint32_t ll = iBuffer->size();

    if (oBuffer->size() != ll)
        oBuffer->resize(ll);

    float * ptr_i = iBuffer->data();
    float * ptr_o = oBuffer->data();

    for (uint32_t i = 0; i < ll - 32; i += 1) {
        execute(ptr_i + i);
        ptr_o[i] = array[0];
    }
}
