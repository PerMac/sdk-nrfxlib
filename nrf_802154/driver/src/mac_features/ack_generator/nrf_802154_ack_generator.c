/*
 * Copyright (c) 2017 - 2022, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @file
 *   This module implements an acknowledgement generator for 802.15.4 radio driver.
 *
 */

#include "nrf_802154_ack_generator.h"

#include <assert.h>

#include "nrf_802154_const.h"
#include "nrf_802154_enh_ack_generator.h"
#include "nrf_802154_imm_ack_generator.h"

typedef enum
{
    FRAME_VERSION_BELOW_2015,
    FRAME_VERSION_2015_OR_ABOVE,
    FRAME_VERSION_INVALID
} frame_version_t;

static frame_version_t frame_version_is_2015_or_above(
    const nrf_802154_frame_parser_data_t * p_frame_data)
{
    switch (nrf_802154_frame_parser_frame_version_get(p_frame_data))
    {
        case FRAME_VERSION_0:
        case FRAME_VERSION_1:
            return FRAME_VERSION_BELOW_2015;

        case FRAME_VERSION_2:
            return FRAME_VERSION_2015_OR_ABOVE;

        default:
            return FRAME_VERSION_INVALID;
    }
}

void nrf_802154_ack_generator_init(void)
{
    // Both generators are initialized to enable sending both Imm-Acks and Enh-Acks.
    nrf_802154_imm_ack_generator_init();
    nrf_802154_enh_ack_generator_init();
}

void nrf_802154_ack_generator_reset(void)
{
    // Both generators are reset to enable sending both Imm-Ack and Enh-Ack.
    nrf_802154_imm_ack_generator_reset();
    nrf_802154_enh_ack_generator_reset();
}

uint8_t * nrf_802154_ack_generator_create(const nrf_802154_frame_parser_data_t * p_frame_data)
{
    // This function should not be called if ACK is not requested.
    assert(nrf_802154_frame_parser_ar_bit_is_set(p_frame_data));

    switch (frame_version_is_2015_or_above(p_frame_data))
    {
        case FRAME_VERSION_BELOW_2015:
            return nrf_802154_imm_ack_generator_create(p_frame_data);

        case FRAME_VERSION_2015_OR_ABOVE:
            return nrf_802154_enh_ack_generator_create(p_frame_data);

        default:
            return NULL;
    }
}
