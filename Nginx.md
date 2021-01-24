# Типы(mime types)

* ## Текст
---
<pre>
text/html                             html htm shtml;
text/css                              css;
text/xml                              xml;
image/gif                             gif;
image/jpeg                            jpeg jpg;
application/javascript                js;
application/atom+xml                  atom;
application/rss+xml                   rss;
text/mathml                           mml;
text/plain                            txt;
text/vnd.sun.j2me.app-descriptor      jad;
text/vnd.wap.wml                      wml;
text/x-component                      htc;
</pre>
---
* ## Изображения
<pre>
image/png                             png;
image/tiff                            tif tiff;
image/vnd.wap.wbmp                    wbmp;
image/x-icon                          ico;
image/x-jng                           jng;
image/x-ms-bmp                        bmp;
image/svg+xml                         svg svgz;
image/webp                            webp;
</pre>
---
* ## Документы, архивы и тд.
<pre>
application/font-woff                 woff;
application/java-archive              jar war ear;
application/json                      json;
application/mac-binhex40              hqx;
application/msword                    doc;
application/pdf                       pdf;
application/postscript                ps eps ai;
application/rtf                       rtf;
application/vnd.apple.mpegurl         m3u8;
application/vnd.ms-excel              xls;
application/vnd.ms-fontobject         eot;
application/vnd.ms-powerpoint         ppt;
application/vnd.wap.wmlc              wmlc;
application/vnd.google-earth.kml+xml  kml;
application/vnd.google-earth.kmz      kmz;
application/x-7z-compressed           7z;
application/x-cocoa                   cco;
application/x-java-archive-diff       jardiff;
application/x-java-jnlp-file          jnlp;
application/x-makeself                run;
application/x-perl                    pl pm;
application/x-pilot                   prc pdb;
application/x-rar-compressed          rar;
application/x-redhat-package-manager  rpm;
application/x-sea                     sea;
application/x-shockwave-flash         swf;
application/x-stuffit                 sit;
application/x-tcl                     tcl tk;
application/x-x509-ca-cert            der pem crt;
application/x-xpinstall               xpi;
application/xhtml+xml                 xhtml;
application/xspf+xml                  xspf;
application/zip                       zip;
</pre>
---
* ## Исполняемые файлы
<pre>
application/octet-stream              bin exe dll;
application/octet-stream              deb;
application/octet-stream              dmg;
application/octet-stream              iso img;
application/octet-stream              msi msp msm;
</pre>
---
* ## Документы
<pre>
application/vnd.openxmlformats-officedocument.wordprocessingml.document    docx;
application/vnd.openxmlformats-officedocument.spreadsheetml.sheet          xlsx;
application/vnd.openxmlformats-officedocument.presentationml.presentation  pptx;
</pre>
* ## Аудио
<pre>
audio/midi                            mid midi kar;
audio/mpeg                            mp3;
audio/ogg                             ogg;
audio/x-m4a                           m4a;
audio/x-realaudio                     ra;
</pre>
---
* ## Видео
<pre>
video/3gpp                            3gpp 3gp;
video/mp2t                            ts;
video/mp4                             mp4;
video/mpeg                            mpeg mpg;
video/quicktime                       mov;
video/webm                            webm;
video/x-flv                           flv;
video/x-m4v                           m4v;
video/x-mng                           mng;
video/x-ms-asf                        asx asf;
video/x-ms-wmv                        wmv;
video/x-msvideo                       avi;
</pre>
# Ошибки
<pre>
StatusContinue           = 100 // RFC 7231, 6.2.1
StatusSwitchingProtocols = 101 // RFC 7231, 6.2.2
StatusProcessing         = 102 // RFC 2518, 10.1
StatusEarlyHints         = 103 // RFC 8297

StatusOK                   = 200 // RFC 7231, 6.3.1
StatusCreated              = 201 // RFC 7231, 6.3.2
StatusAccepted             = 202 // RFC 7231, 6.3.3
StatusNonAuthoritativeInfo = 203 // RFC 7231, 6.3.4
StatusNoContent            = 204 // RFC 7231, 6.3.5
StatusResetContent         = 205 // RFC 7231, 6.3.6
StatusPartialContent       = 206 // RFC 7233, 4.1
StatusMultiStatus          = 207 // RFC 4918, 11.1
StatusAlreadyReported      = 208 // RFC 5842, 7.1
StatusIMUsed               = 226 // RFC 3229, 10.4.1

StatusMultipleChoices  = 300 // RFC 7231, 6.4.1
StatusMovedPermanently = 301 // RFC 7231, 6.4.2
StatusFound            = 302 // RFC 7231, 6.4.3
StatusSeeOther         = 303 // RFC 7231, 6.4.4
StatusNotModified      = 304 // RFC 7232, 4.1
StatusUseProxy         = 305 // RFC 7231, 6.4.5

StatusTemporaryRedirect = 307 // RFC 7231, 6.4.7
StatusPermanentRedirect = 308 // RFC 7538, 3

StatusBadRequest                   = 400 // RFC 7231, 6.5.1
StatusUnauthorized                 = 401 // RFC 7235, 3.1
StatusPaymentRequired              = 402 // RFC 7231, 6.5.2
StatusForbidden                    = 403 // RFC 7231, 6.5.3
StatusNotFound                     = 404 // RFC 7231, 6.5.4
StatusMethodNotAllowed             = 405 // RFC 7231, 6.5.5
StatusNotAcceptable                = 406 // RFC 7231, 6.5.6
StatusProxyAuthRequired            = 407 // RFC 7235, 3.2
StatusRequestTimeout               = 408 // RFC 7231, 6.5.7
StatusConflict                     = 409 // RFC 7231, 6.5.8
StatusGone                         = 410 // RFC 7231, 6.5.9
StatusLengthRequired               = 411 // RFC 7231, 6.5.10
StatusPreconditionFailed           = 412 // RFC 7232, 4.2
StatusRequestEntityTooLarge        = 413 // RFC 7231, 6.5.11
StatusRequestURITooLong            = 414 // RFC 7231, 6.5.12
StatusUnsupportedMediaType         = 415 // RFC 7231, 6.5.13
StatusRequestedRangeNotSatisfiable = 416 // RFC 7233, 4.4
StatusExpectationFailed            = 417 // RFC 7231, 6.5.14
StatusTeapot                       = 418 // RFC 7168, 2.3.3
StatusMisdirectedRequest           = 421 // RFC 7540, 9.1.2
StatusUnprocessableEntity          = 422 // RFC 4918, 11.2
StatusLocked                       = 423 // RFC 4918, 11.3
StatusFailedDependency             = 424 // RFC 4918, 11.4
StatusTooEarly                     = 425 // RFC 8470, 5.2.
StatusUpgradeRequired              = 426 // RFC 7231, 6.5.15
StatusPreconditionRequired         = 428 // RFC 6585, 3
StatusTooManyRequests              = 429 // RFC 6585, 4
StatusRequestHeaderFieldsTooLarge  = 431 // RFC 6585, 5
StatusUnavailableForLegalReasons   = 451 // RFC 7725, 3

StatusInternalServerError           = 500 // RFC 7231, 6.6.1
StatusNotImplemented                = 501 // RFC 7231, 6.6.2
StatusBadGateway                    = 502 // RFC 7231, 6.6.3
StatusServiceUnavailable            = 503 // RFC 7231, 6.6.4
StatusGatewayTimeout                = 504 // RFC 7231, 6.6.5
StatusHTTPVersionNotSupported       = 505 // RFC 7231, 6.6.6
StatusVariantAlsoNegotiates         = 506 // RFC 2295, 8.1
StatusInsufficientStorage           = 507 // RFC 4918, 11.5
StatusLoopDetected                  = 508 // RFC 5842, 7.2
StatusNotExtended                   = 510 // RFC 2774, 7
StatusNetworkAuthenticationRequired = 511 // RFC 6585, 6
</pre>