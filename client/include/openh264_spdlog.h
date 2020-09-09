#ifndef OPENH264_SPDLOG_H
#define OPENH264_SPDLOG_H

#include <spdlog/spdlog.h>
#include "codec/api/svc/codec_api.h"

namespace ST
{

static spdlog::level::level_enum openh264_to_spdlog(int level)
{
    switch (level)
    {
    case WELS_LOG_QUIET:
        return spdlog::level::level_enum::trace;
    case WELS_LOG_ERROR:
        return spdlog::level::level_enum::err;
    case WELS_LOG_WARNING:
        return spdlog::level::level_enum::warn;
    case WELS_LOG_INFO:
        return spdlog::level::level_enum::info;
    case WELS_LOG_DEBUG:
        return spdlog::level::level_enum::debug;
    case WELS_LOG_DETAIL:
        return spdlog::level::level_enum::info;
    default:
        return spdlog::level::level_enum::debug;
    }
}

static void openh264_spdlog(void* /*context*/, int level, const char* message)
{
    spdlog::log(openh264_to_spdlog(level), message);
}

}

#endif