#include "TLSSocket.h"

/**
 * Platform TLSSocket implementation for the mbedtls
 */
bell::TLSSocket::TLSSocket()
{
  mbedtls_net_init(&server_fd);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);
  const char *pers = "euphonium";
  int ret;
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                   (const unsigned char *)pers,
                                   strlen(pers))) != 0)
  {
    BELL_LOG(error, "http_tls",
             "failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
  }
}

void bell::TLSSocket::open(std::string hostUrl, uint16_t port)
{
  int ret;
  if ((ret = mbedtls_net_connect(&server_fd, hostUrl.c_str(), std::to_string(port).c_str(),
                                 MBEDTLS_NET_PROTO_TCP)) != 0)
  {
    BELL_LOG(error, "http_tls", "failed! connect returned %d\n", ret);
  }

  if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
  {

    BELL_LOG(error, "http_tls", "failed! config returned %d\n", ret);
  }

  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_setup(&ssl, &conf);
  if ((ret = mbedtls_ssl_set_hostname(&ssl, "Mbed TLS Server 1")) != 0)
  {
    BELL_LOG(info, "oh", "kocz");
  }
  mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv,
                      NULL);

  while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
  {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
    {
      BELL_LOG(error, "http_tls", "failed! config returned %d\n", ret);
    }
  }
}

size_t bell::TLSSocket::read(uint8_t *buf, size_t len)
{
  return mbedtls_ssl_read(&ssl, buf, len);
}

size_t bell::TLSSocket::write(uint8_t *buf, size_t len)
{
  return mbedtls_ssl_write(&ssl, buf, len);
}

size_t bell::TLSSocket::poll() {
  return mbedtls_ssl_get_bytes_avail(&ssl);
}

void bell::TLSSocket::close()
{
  mbedtls_net_free(&server_fd);
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
}
