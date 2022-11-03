#include <sio/api.h>
#include <sio/compression/zlib.h>
#include <sio/definitions.h>

#include <utility>

namespace podio {
namespace sio_utils {
  // Read the record into a buffer and potentially uncompress it
  inline std::pair<sio::buffer, sio::record_info> readRecord(sio::ifstream& stream, bool decompress = true,
                                                             std::size_t initBufferSize = sio::mbyte) {
    sio::record_info recInfo;
    sio::buffer infoBuffer{sio::max_record_info_len};
    sio::buffer recBuffer{initBufferSize};
    sio::api::read_record_info(stream, recInfo, infoBuffer);
    sio::api::read_record_data(stream, recInfo, recBuffer);

    if (decompress) {
      sio::buffer uncBuffer{recInfo._uncompressed_length};
      sio::zlib_compression compressor;
      compressor.uncompress(recBuffer.span(), uncBuffer);
      return std::make_pair(std::move(uncBuffer), recInfo);
    }

    return std::make_pair(std::move(recBuffer), recInfo);
  }
} // namespace sio_utils
} // namespace podio
