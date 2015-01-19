#pragma once
#include <string>
#include <stdexcept>
#include <iostream>

#define POSITION (core::CodePosition(__FILE__, __LINE__, __FUNCTION__))

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
		std::string toString()const;
	};
	class Exception : public std::exception
	{
	public:
		inline static Exception CreateAndLog(const CodePosition& pos) {
			std::cerr << "FATAL ERROR. The Exception will be thrown! "
				<< pos.toString()
				<< std::endl;
			return Exception();
		}
		inline static Exception CreateAndLog(const CodePosition& pos, const std::string& message) {
			std::cerr << "FATAL ERROR. The Exception will be thrown! "
				<< pos.toString()
				<< " Message: "
				<< message
				<< std::endl;
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