#ifndef HAUNTED_LIB_UTF8_H_
#define HAUNTED_LIB_UTF8_H_

#include <string>
#include <vector>

#include "unicode/unistr.h"

namespace haunted {
	class utf8 {
		public:
			static size_t width(unsigned char);
	};

	class utf8char {
		public:
			int32_t cp;
			utf8char(int32_t cp): cp(cp) {}
			utf8char(): cp(0) {}
			utf8char(const char *, size_t = 1, size_t = 0);
			utf8char(const std::string &str): utf8char(str.c_str(), str.size(), static_cast<size_t>(0)) {}

			operator int32_t() const;
			operator std::string() const;

			friend std::ostream & operator<<(std::ostream &os, const utf8char &input);
			friend std::string operator+(const std::string &, const utf8char &);
			friend std::string operator+(const char *, const utf8char &);
	};

	using utf8chars = std::vector<utf8char>;

	class utf8str {
		private:
			utf8chars unistr;

			static utf8chars decode_utf8(const std::string &);

		public:
			utf8str() {}
			utf8str(const utf8str &other): unistr(other.unistr) {}
			utf8str(const std::string &str): unistr(utf8str::decode_utf8(str)) {}
			utf8str(UChar32 uch): unistr(uch) {}
			utf8str(char ch): utf8str(std::string(1, ch)) {}

			utf8char operator[](ssize_t) const;
			utf8str & operator=(const utf8str &);
			utf8str & operator=(const utf8chars &);
			utf8str & operator=(const std::string &);
			utf8str & operator=(utf8str &&);
			utf8str & operator=(utf8chars &&);
			utf8str & operator=(std::string &&);
			bool operator==(const utf8str &) const;
			operator std::string() const;

			void append(utf8str &);
			void append(const std::string &);
			void append(char);
			std::string substr(ssize_t, ssize_t) const;
			std::string substr(ssize_t) const;
			void insert(ssize_t, const std::string &);
			void insert(ssize_t, char);
			void erase(ssize_t, ssize_t);
			void erase(ssize_t);
			size_t size()   const;
			size_t length() const;
			bool   empty()  const;
			void   clear();
			utf8chars::iterator begin();
			utf8chars::iterator end();

			friend std::ostream & operator<<(std::ostream &os, const utf8str &input);
			friend std::string operator+(const std::string &, const utf8str &);
			friend std::string operator+(const char *, const utf8str &);

			template <typename T>
			utf8str operator+(const T &other) {
				utf8str copy(*this);
				copy += other;
				return copy;
			}

			template <typename T>
			utf8str & operator+=(const T &other) {
				append(other);
				return *this;
			}
	};
}

#endif