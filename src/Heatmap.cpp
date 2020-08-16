/* XMRig
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "Heatmap.h"
#include "3rdparty/heatmap/heatmap.h"
#include "3rdparty/heatmap/colorschemes/Spectral.h"
#include "3rdparty/heatmap/lodepng.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/Env.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"
#include "base/tools/Chrono.h"
#include "Job.h"


#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>


#ifdef XMRIG_LIBPNG
#   include <png.h>
#endif


namespace xmrig {


static const char *kTag = GREEN_BG_BOLD(WHITE_BOLD_S " heatmap ");


#ifdef XMRIG_LIBPNG
static void writepng_libpng(const char *fileName, const uint8_t *data, size_t width, size_t height)
{
    // NULLS are user error/warning functions.
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        throw std::runtime_error("error initializing libpng write struct");
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_write_struct(&png_ptr, nullptr);

       throw std::runtime_error("error initializing libpng info struct");
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);

        throw std::runtime_error("error in setjmp!?");
    }

    FILE *fp = fopen(fileName, "wb");
    if (!fp) {
        png_destroy_write_struct(&png_ptr, &info_ptr);

        throw std::runtime_error(std::string("error writing ") + fileName + ": " + strerror(errno));
    }

    png_init_io(png_ptr, fp);
    png_set_filter(png_ptr, 0, PNG_FILTER_NONE | PNG_FILTER_VALUE_NONE);
    png_set_compression_level(png_ptr, 6);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    auto *row_pointers = reinterpret_cast<png_byte**>(png_malloc(png_ptr, height * sizeof(png_bytep)));
    for (size_t y = 0 ; y < height ; ++y) {
        row_pointers[y] = const_cast<png_bytep>(data + y * width * 4);
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

    png_free(png_ptr, row_pointers);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}
#endif


static void writepng_lodepng(const char *fileName, const std::vector<uint8_t> &in, uint32_t width, uint32_t height)
{
    if (uint32_t error = lodepng::encode(fileName, in, width, height)) {
        throw std::runtime_error(lodepng_error_text(error));
    }
}


static void writepng_auto(const char *fileName, const std::vector<uint8_t> &in, uint32_t width, uint32_t height)
{
#   ifdef XMRIG_LIBPNG
    try {
        writepng_libpng(fileName, in.data(), width, height);
    } catch (std::exception &ex) {
        LOG_ERR("%s " RED("libpng error: ") RED_BOLD("%s "), Heatmap::tag(), ex.what());
        LOG_INFO("%s " MAGENTA_BOLD("trying switch to lodepng"), Heatmap::tag());

        writepng_lodepng(fileName, in, width, height);
    }
#   else
    writepng_lodepng(fileName, in, width, height);
#   endif
}


template<bool CUSTOM_Y>
static void nonces_heatmap(const std::vector<uint32_t>::const_iterator &begin, const std::vector<uint32_t>::const_iterator &end, const char *fileName, uint32_t x, uint32_t height, uint32_t radius, uint32_t min = 0, uint32_t max = 0)
{
    constexpr uint32_t MAX = std::numeric_limits<uint32_t>::max();
    const uint32_t width   = std::ceil(static_cast<float>(std::distance(begin, end)) / x);

    auto hm    = heatmap_new(width, height);
    auto stamp = heatmap_stamp_gen(radius);
    size_t i   = 0;

    if (CUSTOM_Y) {
        uint32_t nonce       = 0;
        const size_t range = max - min;

        for (auto it = begin; it != end; ++it) {
            nonce = *it;

            if (nonce >= min && nonce <= max) {
                heatmap_add_point_with_stamp(hm, i / x, (max - nonce) / (range / height), stamp);
            }

            ++i;
        }
    }
    else {
        for (auto it = begin; it != end; ++it) {
            heatmap_add_point_with_stamp(hm, i / x, (MAX - *it) / (MAX / height), stamp);
            ++i;
        }
    }

    heatmap_stamp_free(stamp);

    std::vector<uint8_t> image(width * height * 4);
    heatmap_render_to(hm, heatmap_cs_Spectral_mixed_exp, &image[0]);

    heatmap_free(hm);

    writepng_auto(fileName, image, width, height);
}


template<bool START>
static inline std::vector<uint32_t>::const_iterator get_iterator(const std::vector<uint32_t> &nonces, int32_t pos)
{
    if (pos > 0) {
        return static_cast<size_t>(pos) < nonces.size() ? nonces.cbegin() + pos : nonces.cend();
    }

    if (pos < 0) {
        pos = std::abs(pos);

        return static_cast<size_t>(pos) < nonces.size() ? nonces.cend() - pos : nonces.cbegin();
    }

    return START ? nonces.cbegin() : nonces.cend();
}


} // namespace xmrig


xmrig::Heatmap::Heatmap(const Job &job, const std::vector<uint32_t> &nonces) :
    m_job(job),
    m_nonces(nonces),
    m_x(job.x())
{
}


void xmrig::Heatmap::render()
{
    if (m_nonces.empty()) {
        throw std::runtime_error("nonces array is empty");
    }

    if (!m_job.height()) {
        throw std::runtime_error("image height is not specified");
    }

    const auto begin = get_iterator<true>(m_nonces, m_job.start());
    if (begin == m_nonces.cend()) {
        throw std::runtime_error("invalid start height");
    }

    const auto end = get_iterator<false>(m_nonces, m_job.end());
    if (begin >= end) {
        throw std::runtime_error("invalid end height");
    }

    m_count = std::distance(begin, end);
    m_x     = m_job.x() ? m_job.x() : std::max<uint32_t>(std::ceil(static_cast<float>(m_count) / static_cast<float>((m_job.height() * 2))), 1);
    m_width = std::ceil(static_cast<float>(std::distance(begin, end)) / m_x);

    if (static_cast<size_t>(m_width) * m_job.height() * 4 > std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("image too big");
    }

    const uint64_t ts = Chrono::steadyMSecs();
    genName(begin, end);

#   ifdef XMRIG_LIBPNG
    LOG_V1("%s " BLACK_BOLD("libpng/%s"), tag(), PNG_LIBPNG_VER_STRING);
#   endif

    LOG_INFO("%s " MAGENTA_BOLD("render ") WHITE_BOLD("\"%s\" ") CYAN_BOLD("%ux%u") " x:" CYAN_BOLD("%u"), tag(), m_name.data(), m_width, m_job.height(), m_x);

    if (m_job.minNonce() == 0 && m_job.maxNonce() == 0) {
        nonces_heatmap<false>(begin, end, m_name, m_x, m_job.height(), m_job.radius());
    }
    else {
        nonces_heatmap<true>(begin, end, m_name, m_x, m_job.height(), m_job.radius(), m_job.minNonce(), m_job.maxNonce());
    }

    saveInfo();

    LOG_INFO("%s " GREEN_BOLD("done ") BLACK_BOLD("(%" PRIu64 "ms)"), tag(), Chrono::steadyMSecs() - ts);
}


const char *xmrig::Heatmap::tag()
{
    return kTag;
}


void xmrig::Heatmap::genName(const std::vector<uint32_t>::const_iterator &begin, const std::vector<uint32_t>::const_iterator &end)
{
    m_start = std::distance(m_nonces.cbegin(), begin);
    m_end   = std::distance(m_nonces.cbegin(), end) - 1;

#   ifdef XMRIG_FEATURE_ENV
    std::map<String, String> variables = {
        { "WIDTH",      std::to_string(m_width).c_str() },
        { "HEIGHT",     std::to_string(m_job.height()).c_str() },
        { "COUNT",      std::to_string(m_count).c_str() },
        { "START",      std::to_string(m_start).c_str() },
        { "END",        std::to_string(m_end).c_str() },
        { "MIN_NONCE",  std::to_string(m_job.minNonce()).c_str() },
        { "MAX_NONCE",  std::to_string(m_job.maxNonce()).c_str() },
        { "RADIUS",     std::to_string(m_job.radius()).c_str() },
        { "X",          std::to_string(m_x).c_str() },
    };

    if (Log::verbose()) {
        for (const auto &kv : variables) {
            LOG_INFO("%s %-10s" CYAN_BOLD("%s"), tag(), kv.first.data(), kv.second.data());
        }
    }

    m_name = Env::expand(m_job.name(), variables);
#   else
    m_name = m_job.name();
#   endif
}


void xmrig::Heatmap::saveInfo()
{
    using namespace rapidjson;

    Document doc(kObjectType);
    auto &allocator = doc.GetAllocator();

    doc.AddMember(StringRef(Job::kName),        m_name.toJSON(), allocator);
    doc.AddMember(StringRef(Job::kWidth),       m_width, allocator);
    doc.AddMember(StringRef(Job::kHeight),      m_job.height(), allocator);
    doc.AddMember(StringRef(Job::kCount),       m_count, allocator);
    doc.AddMember(StringRef(Job::kStart),       m_start, allocator);
    doc.AddMember(StringRef(Job::kEnd),         m_end, allocator);
    doc.AddMember(StringRef(Job::kMinNonce),    m_job.minNonce(), allocator);
    doc.AddMember(StringRef(Job::kMaxNonce),    m_job.maxNonce(), allocator);
    doc.AddMember(StringRef(Job::kRadius),      m_job.radius(), allocator);
    doc.AddMember(StringRef(Job::kX),           m_x, allocator);

    Json::save(m_job.infoFile().isEmpty() ? (std::string(m_name) + ".json").c_str() : m_job.infoFile().data(), doc);
}
