#pragma once
#include <string>
#include "ProcessLogger.h"
#include <stdexcept>
#include <sstream>

#define POSITION (utils::CodePosition(__FILE__, __LINE__, __FUNCTION__))

namespace utils
{
	struct CodePosition
	{
		CodePosition(const char* file, int line, const char* function)
			: File(file), Line(line), Function(function) {
		}
		const char* File;
		const int Line;
		const char* Function;
		std::string toString()const{
			std::stringstream ss;
			ss << File << " line" << Line << " function: " << Function << std::endl;
			return ss.str();
		}
	};

	class Exception : public std::exception
	{
	public:
		inline static Exception CreateAndLog(const CodePosition& pos) {
			logger << "FATAL ERROR. The Exception will be thrown! "
				<< pos.toString();
			return Exception();
		}
		inline static Exception CreateAndLog(const CodePosition& pos, const std::string& message) {
			logger << "FATAL ERROR. The Exception will be thrown! "
				<< pos.toString()
				<< " Message: "
				<< message;
			return Exception(message);
		}
	public:
		virtual const char* what() const throw () {
			return _message.c_str();
		}
		const std::string& GetErrorMessage() const {
			return _message;
		}
		~Exception() throw () {
		};
	protected:
		Exception() {
		}
		Exception(const char*& message) : _message(std::string(message)) {
		}
		Exception(const std::string& message) : _message(message) {
		}
	private:
		std::string _message;
	};
}