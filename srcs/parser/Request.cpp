#include "parser/Request.hpp"

Request::Request() {
	mime_types["html"] = "text/html";
	mime_types["htm"] = "text/html";
	mime_types["shtml"] = "text/html";
	mime_types["css"] = "text/css";
	mime_types["xml"] = "text/xml";
	mime_types["gif"] = "image/gif";
	mime_types["jpeg"] = "image/jpeg";
	mime_types["jpg"] = "image/jpeg";
	mime_types["js"] = "application/javascript";
	mime_types["atom"] = "application/atom+xml";
	mime_types["rss"] = "application/rss+xml";
	mime_types["mml"] = "text/mathml";
	mime_types["txt"] = "text/plain";
	mime_types["jad"] = "text/vnd.sun.j2me.app-descriptor";
	mime_types["wml"] = "text/vnd.wap.wml";
	mime_types["htc"] = "text/x-component";
	mime_types["png"] = "image/png";
	mime_types["tif"] = "image/tiff";
	mime_types["tiff"] = "image/tiff";
	mime_types["wbmp"] = "image/vnd.wap.wbmp";
	mime_types["ico"] = "image/x-icon";
	mime_types["jng"] = "image/x-jng";
	mime_types["bmp"] = "image/x-ms-bmp";
	mime_types["svg"] = "image/svg+xml";
	mime_types["svgz"] = "image/svg+xml";
	mime_types["webp"] = "image/webp";
	mime_types["woff"] = "application/font-woff";
	mime_types["jar"] = "application/java-archive";
	mime_types["war"] = "application/java-archive";
	mime_types["ear"] = "application/java-archive";
	mime_types["json"] = "application/json";
	mime_types["hqx"] = "application/mac-binhex40";
	mime_types["doc"] = "application/msword";
	mime_types["pdf"] = "application/pdf";
	mime_types["ps"] = "application/postscript";
	mime_types["eps"] = "application/postscript";
	mime_types["ai"] = "application/postscript";
	mime_types["rtf"] = "application/rtf";
	mime_types["m3u8"] = "application/vnd.apple.mpegurl";
	mime_types["xls"] = "application/vnd.ms-excel";
	mime_types["eot"] = "application/vnd.ms-fontobject";
	mime_types["ppt"] = "application/vnd.ms-powerpoint";
	mime_types["wmlc"] = "application/vnd.wap.wmlc";
	mime_types["kml"] = "application/vnd.google-earth.kml+xml";
	mime_types["kmz"] = "application/vnd.google-earth.kmz";
	mime_types["7z"] = "application/x-7z-compressed";
	mime_types["cco"] = "application/x-cocoa";
	mime_types["jardiff"] = "application/x-java-archive-diff";
	mime_types["jnlp"] = "application/x-java-jnlp-file";
	mime_types["run"] = "application/x-makeself";
	mime_types["pl"] = "application/x-perl";
	mime_types["pm"] = "application/x-perl";
	mime_types["prc"] = "application/x-pilot";
	mime_types["pdb"] = "application/x-pilot";
	mime_types["rar"] = "application/x-rar-compressed";
	mime_types["rpm"] = "application/x-redhat-package-manager";
	mime_types["sea"] = "application/x-sea";
	mime_types["swf"] = "application/x-shockwave-flash";
	mime_types["sit"] = "application/x-stuffit";
	mime_types["tcl"] = "application/x-tcl";
	mime_types["tk"] = "application/x-tcl";
	mime_types["der"] = "application/x-x509-ca-cert";
	mime_types["pem"] = "application/x-x509-ca-cert";
	mime_types["crt"] = "application/x-x509-ca-cert";
	mime_types["xpi"] = "application/x-xpinstall";
	mime_types["xhtml"] = "application/xhtml+xml";
	mime_types["xspf"] = "application/xspf+xml";
	mime_types["zip"] = "application/zip";
	mime_types["bin"] = "application/octet-stream";
	mime_types["exe"] = "application/octet-stream";
	mime_types["dll"] = "application/octet-stream";
	mime_types["deb"] = "application/octet-stream";
	mime_types["dmg"] = "application/octet-stream";
	mime_types["iso"] = "application/octet-stream";
	mime_types["img"] = "application/octet-stream";
	mime_types["msi"] = "application/octet-stream";
	mime_types["msp"] = "application/octet-stream";
	mime_types["msm"] = "application/octet-stream";
	mime_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_types["mid"] = "audio/midi";
	mime_types["midi"] = "audio/midi";
	mime_types["kar"] = "audio/midi";
	mime_types["mp3"] = "audio/mpeg";
	mime_types["ogg"] = "audio/ogg";
	mime_types["m4a"] = "audio/x-m4a";
	mime_types["ra"] = "audio/x-realaudio";
	mime_types["3gpp"] = "video/3gpp";
	mime_types["3gp"] = "video/3gpp";
	mime_types["ts"] = "video/mp2t";
	mime_types["mp4"] = "video/mp4";
	mime_types["mpeg"] = "video/mpeg";
	mime_types["mpg"] = "video/mpeg";
	mime_types["mov"] = "video/quicktime";
	mime_types["webm"] = "video/webm";
	mime_types["flv"] = "video/x-flv";
	mime_types["m4v"] = "video/x-m4v";
	mime_types["mng"] = "video/x-mng";
	mime_types["asx"] = "video/x-ms-asf";
	mime_types["asf"] = "video/x-ms-asf";
	mime_types["wmv"] = "video/x-ms-wmv";
	mime_types["avi"] = "video/x-msvideo";
}

Request::~Request() {}

void Request::parseRequest(const std::string& data) {

	std::string delim("\r\n");
	std::vector<std::string> result = split(data, delim);

	for (size_t i = 0; i < result.size(); i++)
		std::cout << "result[" << i << "] = " << result[i]  << std::endl << std::flush;

}

