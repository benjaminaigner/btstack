/*
 * Copyright (C) 2023 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "hfp_codec.c"
 
// *****************************************************************************
//
// HFP Codec
//
// *****************************************************************************

#include "btstack_config.h"

#include <string.h>

#include "hfp_codec.h"
#include "btstack_debug.h"

#ifdef ENABLE_HFP_WIDE_BAND_SPEECH
#include "btstack_sbc.h"
#define FRAME_SIZE_MSBC 57
static void hfp_codec_encode_msbc(hfp_codec_t * hfp_codec, int16_t * pcm_samples);
#endif

#ifdef ENABLE_HFP_SUPER_WIDE_BAND_SPEECH
#define LC3_SWB_OCTETS_PER_FRAME   58
static void hfp_codec_encode_lc3swb(hfp_codec_t * hfp_codec, int16_t * pcm_samples);
#endif

#include "hfp_codec.h"

void hfp_codec_init(hfp_codec_t * hfp_codec, uint8_t codec_id){
    memset(hfp_codec, 0, sizeof(hfp_codec_t));
    hfp_h2_framing_init(&hfp_codec->h2_framing);
    switch (codec_id){
#ifdef ENABLE_HFP_WIDE_BAND_SPEECH
        case HFP_CODEC_MSBC:
            hfp_codec->samples_per_frame = 120;
            hfp_codec->encode = &hfp_codec_encode_msbc;
            btstack_sbc_encoder_init(&hfp_codec->msbc_state, SBC_MODE_mSBC, 16, 8, SBC_ALLOCATION_METHOD_LOUDNESS, 16000, 26, SBC_CHANNEL_MODE_MONO);
            break;
#endif
#ifdef ENABLE_HFP_SUPER_WIDE_BAND_SPEECH
        case HFP_CODEC_LC3_SWB:
            hfp_codec->samples_per_frame = 240;
            hfp_codec->encode = &hfp_codec_encode_lc3swb;
            // init lc3 encoder
            hfp_codec->lc3_encoder = btstack_lc3_encoder_google_init_instance(&hfp_codec->lc3_encoder_context);
            hfp_codec->lc3_encoder->configure(&hfp_codec->lc3_encoder_context, 32000, BTSTACK_LC3_FRAME_DURATION_7500US, LC3_SWB_OCTETS_PER_FRAME);
            break;
#endif
        default:
            btstack_assert(false);
            break;
    }
}

bool hfp_codec_can_encode_audio_frame_now(const hfp_codec_t * hfp_codec){
    return hfp_codec->write_pos <= SCO_FRAME_SIZE;
}

uint16_t hfp_codec_num_audio_samples_per_frame(const hfp_codec_t * hfp_codec){
    return hfp_codec->samples_per_frame;
}

#ifdef ENABLE_HFP_WIDE_BAND_SPEECH
static void hfp_codec_encode_msbc(hfp_codec_t * hfp_codec, int16_t * pcm_samples){
    // Encode SBC Frame
    btstack_sbc_encoder_process_data(pcm_samples);
    (void)memcpy(&hfp_codec->sco_packet[hfp_codec->write_pos], btstack_sbc_encoder_sbc_buffer(), FRAME_SIZE_MSBC);
    hfp_codec->write_pos += FRAME_SIZE_MSBC;
    // Final padding to use SCO_FRAME_SIZE bytes
    hfp_codec->sco_packet[hfp_codec->write_pos++] = 0;
}
#endif

#ifdef ENABLE_HFP_SUPER_WIDE_BAND_SPEECH
static void hfp_codec_encode_lc3swb(hfp_codec_t * hfp_codec, int16_t * pcm_samples){
    // Encode LC3 Frame
    hfp_codec->lc3_encoder->encode_signed_16(&hfp_codec->lc3_encoder_context, pcm_samples, 1, &hfp_codec->sco_packet[hfp_codec->write_pos]);
    hfp_codec->write_pos += LC3_SWB_OCTETS_PER_FRAME;
}
#endif

void hfp_codec_encode_audio_frame(hfp_codec_t * hfp_codec, int16_t * pcm_samples){
    btstack_assert(hfp_codec_can_encode_audio_frame_now(hfp_codec));
    uint16_t offset = hfp_codec->write_pos;
    // Synchronization Header H2
    hfp_h2_framing_add_header(&hfp_codec->h2_framing, &hfp_codec->sco_packet[hfp_codec->write_pos]);
    hfp_codec->write_pos += 2;
    // encode
    hfp_codec->encode(hfp_codec, pcm_samples);
    log_info("Encode frame, read %u, write %u", hfp_codec->read_pos, hfp_codec->write_pos);
}

uint16_t hfp_codec_num_bytes_available(const hfp_codec_t * hfp_codec){
    return hfp_codec->write_pos - hfp_codec->read_pos;
}

void hfp_codec_read_from_stream(hfp_codec_t * hfp_codec, uint8_t * buf, uint16_t size){
    uint16_t num_bytes_available = hfp_codec_num_bytes_available(hfp_codec);
    btstack_assert(num_bytes_available >= size);

    uint16_t num_bytes_to_copy = btstack_min(num_bytes_available, size);

    memcpy(buf, &hfp_codec->sco_packet[hfp_codec->read_pos], num_bytes_to_copy);
    hfp_codec->read_pos += num_bytes_to_copy;

    // reset buffer
    if (hfp_codec->read_pos == hfp_codec->write_pos){
        hfp_codec->read_pos = 0;
        hfp_codec->write_pos = 0;
    }

    log_info("Read %u from stream, read %u, write %u", size, hfp_codec->read_pos, hfp_codec->write_pos);
}

void hfp_codec_deinit(hfp_codec_t * hfp_codec){
    UNUSED(hfp_codec);
}
