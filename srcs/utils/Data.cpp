#include "utils/Data.hpp"

Data::Data() {

	/*
	** Mime типы
	*/
	_mimeMap["html"] = "text/html";
	_mimeMap["htm"] = "text/html";
	_mimeMap["shtml"] = "text/html";
	_mimeMap["css"] = "text/css";
	_mimeMap["xml"] = "text/xml";
	_mimeMap["gif"] = "image/gif";
	_mimeMap["jpeg"] = "image/jpeg";
	_mimeMap["jpg"] = "image/jpeg";
	_mimeMap["js"] = "application/javascript";
	_mimeMap["atom"] = "application/atom+xml";
	_mimeMap["rss"] = "application/rss+xml";
	_mimeMap["mml"] = "text/mathml";
	_mimeMap["txt"] = "text/plain";
	_mimeMap["jad"] = "text/vnd.sun.j2me.app-descriptor";
	_mimeMap["wml"] = "text/vnd.wap.wml";
	_mimeMap["htc"] = "text/x-component";
	_mimeMap["png"] = "image/png";
	_mimeMap["tif"] = "image/tiff";
	_mimeMap["tiff"] = "image/tiff";
	_mimeMap["wbmp"] = "image/vnd.wap.wbmp";
	_mimeMap["ico"] = "image/x-icon";
	_mimeMap["jng"] = "image/x-jng";
	_mimeMap["bmp"] = "image/x-ms-bmp";
	_mimeMap["svg"] = "image/svg+xml";
	_mimeMap["svgz"] = "image/svg+xml";
	_mimeMap["webp"] = "image/webp";
	_mimeMap["woff"] = "application/font-woff";
	_mimeMap["jar"] = "application/java-archive";
	_mimeMap["war"] = "application/java-archive";
	_mimeMap["ear"] = "application/java-archive";
	_mimeMap["json"] = "application/json";
	_mimeMap["hqx"] = "application/mac-binhex40";
	_mimeMap["doc"] = "application/msword";
	_mimeMap["pdf"] = "application/pdf";
	_mimeMap["ps"] = "application/postscript";
	_mimeMap["eps"] = "application/postscript";
	_mimeMap["ai"] = "application/postscript";
	_mimeMap["rtf"] = "application/rtf";
	_mimeMap["m3u8"] = "application/vnd.apple.mpegurl";
	_mimeMap["xls"] = "application/vnd.ms-excel";
	_mimeMap["eot"] = "application/vnd.ms-fontobject";
	_mimeMap["ppt"] = "application/vnd.ms-powerpoint";
	_mimeMap["wmlc"] = "application/vnd.wap.wmlc";
	_mimeMap["kml"] = "application/vnd.google-earth.kml+xml";
	_mimeMap["kmz"] = "application/vnd.google-earth.kmz";
	_mimeMap["7z"] = "application/x-7z-compressed";
	_mimeMap["cco"] = "application/x-cocoa";
	_mimeMap["jardiff"] = "application/x-java-archive-diff";
	_mimeMap["jnlp"] = "application/x-java-jnlp-file";
	_mimeMap["run"] = "application/x-makeself";
	_mimeMap["pl"] = "application/x-perl";
	_mimeMap["pm"] = "application/x-perl";
	_mimeMap["prc"] = "application/x-pilot";
	_mimeMap["pdb"] = "application/x-pilot";
	_mimeMap["rar"] = "application/x-rar-compressed";
	_mimeMap["rpm"] = "application/x-redhat-package-manager";
	_mimeMap["sea"] = "application/x-sea";
	_mimeMap["swf"] = "application/x-shockwave-flash";
	_mimeMap["sit"] = "application/x-stuffit";
	_mimeMap["tcl"] = "application/x-tcl";
	_mimeMap["tk"] = "application/x-tcl";
	_mimeMap["der"] = "application/x-x509-ca-cert";
	_mimeMap["pem"] = "application/x-x509-ca-cert";
	_mimeMap["crt"] = "application/x-x509-ca-cert";
	_mimeMap["xpi"] = "application/x-xpinstall";
	_mimeMap["xhtml"] = "application/xhtml+xml";
	_mimeMap["xspf"] = "application/xspf+xml";
	_mimeMap["zip"] = "application/zip";
	_mimeMap["bin"] = "application/octet-stream";
	_mimeMap["exe"] = "application/octet-stream";
	_mimeMap["dll"] = "application/octet-stream";
	_mimeMap["deb"] = "application/octet-stream";
	_mimeMap["dmg"] = "application/octet-stream";
	_mimeMap["iso"] = "application/octet-stream";
	_mimeMap["img"] = "application/octet-stream";
	_mimeMap["msi"] = "application/octet-stream";
	_mimeMap["msp"] = "application/octet-stream";
	_mimeMap["msm"] = "application/octet-stream";
	_mimeMap["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mimeMap["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mimeMap["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mimeMap["mid"] = "audio/midi";
	_mimeMap["midi"] = "audio/midi";
	_mimeMap["kar"] = "audio/midi";
	_mimeMap["mp3"] = "audio/mpeg";
	_mimeMap["ogg"] = "audio/ogg";
	_mimeMap["m4a"] = "audio/x-m4a";
	_mimeMap["ra"] = "audio/x-realaudio";
	_mimeMap["3gpp"] = "video/3gpp";
	_mimeMap["3gp"] = "video/3gpp";
	_mimeMap["ts"] = "video/mp2t";
	_mimeMap["mp4"] = "video/mp4";
	_mimeMap["mpeg"] = "video/mpeg";
	_mimeMap["mpg"] = "video/mpeg";
	_mimeMap["mov"] = "video/quicktime";
	_mimeMap["webm"] = "video/webm";
	_mimeMap["flv"] = "video/x-flv";
	_mimeMap["m4v"] = "video/x-m4v";
	_mimeMap["mng"] = "video/x-mng";
	_mimeMap["asx"] = "video/x-ms-asf";
	_mimeMap["asf"] = "video/x-ms-asf";
	_mimeMap["wmv"] = "video/x-ms-wmv";
	_mimeMap["avi"] = "video/x-msvideo";

	/*
	** Коды состояния
	 */
	_httpMap["200"] = new Node(e_success, "OK");
	_httpMap["201"] = new Node(e_success, "Created");
	_httpMap["400"] = new Node(e_clientError, "Bad Request");
	_httpMap["404"] = new Node(e_clientError, "Not Found");

	static std::string errorsDir = "./errors/";
	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end(); httpMapIt++) {
		if (isErrorStatus(httpMapIt))
			httpMapIt->second->setPath(errorsDir + httpMapIt->first + ".html");
	}


}

Data::~Data() {
	for (_httpMapIt httpMapIt = _httpMap.begin(); httpMapIt != _httpMap.end();) {
		delete (*httpMapIt).second;
		_httpMap.erase(httpMapIt++);
	}
}

const std::map<std::string, std::string>& Data::getMimeMap() const {
	return (_mimeMap);
}

const Data::_httpMapType &Data::getHttpMap() const {
	return (_httpMap);
}

std::string Data::getMessage(const HttpStatusCode &httpStatusCode) const {
	return (_httpMap.find(httpStatusCode.getStatusCode())->second->getName());
}

std::string Data::getErrorPath(const HttpStatusCode &httpStatusCode) const {
	return (_httpMap.find(httpStatusCode.getStatusCode())->second->getPath());
}

bool Data::isErrorStatus(const HttpStatusCode *httpStatusCode) const {
	static int type;

	type = _httpMap.find(httpStatusCode->getStatusCode())->second->getType();
	return (type == e_clientError || type == e_serverError);
}

bool Data::isErrorStatus(const Data::_httpMapIt &httpMapIt) const {
	static int type;

	type = httpMapIt->second->getType();
	return (type == e_clientError || type == e_serverError);
}
