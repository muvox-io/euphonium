#include "NanoPBHelper.h"

static bool vectorWrite(pb_ostream_t *stream, const pb_byte_t *buf, size_t count)
{
    size_t i;
    auto *dest = reinterpret_cast<std::vector<uint8_t> *>(stream->state);

    dest->insert(dest->end(), buf, buf + count);

    return true;
}

pb_ostream_t pb_ostream_from_vector(std::vector<uint8_t> &vec)
{
    pb_ostream_t stream;

    stream.callback = &vectorWrite;
    stream.state = &vec;
    stream.max_size = 100000;
    stream.bytes_written = 0;

    return stream;
}

std::vector<uint8_t> pbEncode(const pb_msgdesc_t *fields, const void *src_struct)
{
    std::vector<uint8_t> vecData(0);
    pb_ostream_t stream = pb_ostream_from_vector(vecData);
    pb_encode(&stream, fields, src_struct);

    return vecData;
}

void packString(char *&dst, std::string stringToPack)
{
    dst = (char *)malloc((strlen(stringToPack.c_str()) + 1) * sizeof(char));
    strcpy(dst, stringToPack.c_str());
}

pb_bytes_array_t* vectorToPbArray(const std::vector<uint8_t>& vectorToPack)
{
    auto size = static_cast<pb_size_t>(vectorToPack.size());
    auto result = static_cast<pb_bytes_array_t *>(
        malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(size)));
    result->size = size;
    memcpy(result->bytes, vectorToPack.data(), size);
    return result;
}

void pbPutString(const std::string &stringToPack, char* dst) {
    stringToPack.copy(dst, stringToPack.size());
    dst[stringToPack.size()] = '\0';
}

void pbPutCharArray(const char * stringToPack, char* dst) {
    // copy stringToPack into dst
    strcpy(dst, stringToPack);
    //dst[sizeof(stringToPack)-1] = '\0';
}

void pbPutBytes(const std::vector<uint8_t> &data, pb_bytes_array_t &dst) {
    dst.size = data.size();
    std::copy(data.begin(), data.end(), dst.bytes);
}

std::vector<uint8_t> pbArrayToVector(pb_bytes_array_t* pbArray) {
    return std::vector<uint8_t>(pbArray->bytes, pbArray->bytes + pbArray->size);
}

const char *pb_encode_to_string(const pb_msgdesc_t *fields, const void *data) {
	size_t len;
	pb_get_encoded_size(&len, fields, data);
	auto *buf = static_cast<uint8_t *>(malloc(len + 1));
	auto ostream = pb_ostream_from_buffer(buf, len);
	pb_encode(&ostream, fields, data);
	buf[len] = '\0';
	return reinterpret_cast<const char *>(buf);
}

static bool pb_read_from_http(pb_istream_t *stream, pb_byte_t *buf, size_t count) {
	auto *response = (bell::HTTPClient::HTTPResponse *)stream->state;
	size_t len = response->read(buf, count, /* wait */ true);
	if (response->isComplete)
		stream->bytes_left = count; // count is subtracted after the callback
	return len == count;
}

pb_istream_t pb_istream_from_http(bell::HTTPClient::HTTPResponse *response, size_t length) {
	if (!length)
		length = response->contentLength;
	if (!length)
		length = SIZE_MAX;
	return {
		.callback = &pb_read_from_http,
		.state = response,
		.bytes_left = length,
	};
}
