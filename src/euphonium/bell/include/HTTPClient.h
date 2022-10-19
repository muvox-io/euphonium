#ifndef BELL_HTTP_CLIENT
#define BELL_HTTP_CLIENT

#include "BellSocket.h"
#include "ByteStream.h"
#include "TCPSocket.h"
#include "TLSSocket.h"
#include <map>
#include <memory>
#include <string>

#define BUF_SIZE 128

namespace bell {
class HTTPClient {
  public:
	enum HTTPMethod {
		GET,
		POST,
	};

	struct HTTPRequest {
		HTTPMethod method = HTTPMethod::GET;
		std::string url;
		const char *body = nullptr;
		const char *contentType = nullptr;
		std::map<std::string, std::string> headers;
		int maxRedirects = -1;
		std::ostream *dumpFs = nullptr;
		std::ostream *dumpRawFs = nullptr;
	};

	struct HTTPResponse : public ByteStream {
		std::shared_ptr<bell::Socket> socket;

		std::map<std::string, std::string> headers;

		uint16_t statusCode;
		uint32_t contentLength;
		std::string contentType;
		std::string location;
		bool isChunked = false;
		bool isGzip = false;
		bool isComplete = false;
		bool isRedirect = false;
		uint8_t redirectCount = 0;
		std::ostream *dumpFs = nullptr;
		std::ostream *dumpRawFs = nullptr;

		~HTTPResponse();
		void close() override;

		void readHeaders();
		uint32_t read(char *dst, uint32_t len, bool wait = false);
		std::string readToString();

		inline size_t skip(size_t len) override {
			return (size_t)read((char *)nullptr, len);
		}
		inline size_t read(uint8_t *dst, size_t len) override {
			return (size_t)read((char *)dst, len, false);
		}
		inline uint32_t read(uint8_t *dst, uint32_t len, bool wait) {
			return read((char *)dst, len, wait);
		}
		inline size_t size() override {
			return (size_t)contentLength;
		}
		inline size_t position() override {
			return (size_t)bodyRead;
		}

	  private:
		char *buf = nullptr;	// allocated buffer
		char *bufPtr = nullptr; // reading pointer within buf
		uint32_t bodyRead = 0;
		uint32_t bufRemaining = 0;
		uint32_t chunkRemaining = 0;
		bool isStreaming = false;
		uint32_t readRaw(char *dst);
		bool skipRaw(uint32_t len, bool dontRead = false);
	};

	typedef std::unique_ptr<struct HTTPClient::HTTPResponse> HTTPResponse_t;

  private:
	static HTTPResponse_t executeImpl(const struct HTTPRequest &request, HTTPResponse_t response);
	static bool readHeader(const char *&header, const char *name);

  public:
	static HTTPResponse_t execute(const struct HTTPRequest &request);
};
typedef std::unique_ptr<struct HTTPClient::HTTPResponse> HTTPResponse_t;
} // namespace bell

#endif
