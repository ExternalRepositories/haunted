#ifndef HAUNTED_UI_TEXTBOX_H_
#define HAUNTED_UI_TEXTBOX_H_

#include <deque>
#include <functional>
#include <string>
#include <utility>

#include "haunted/ui/colored_control.h"
#include "haunted/core/terminal.h"
#include "haunted/core/util.h"

#include "lib/formicine/performance.h"

namespace haunted::tests {
	class maintest;
}

namespace haunted::ui {
	class textbox;

	/**
	 * Represents a line of text. This is a base class that includes a continuation column; subclasses are responsible
	 * for producing the actual text. (Note that the text is assumed to contain no newlines.)
	 * In irssi, messages that are too wide for a single line are wrapped; each new line begins at the same column as
	 * the message did, after the timestamp and nick indicator. This wrapper makes a generalized version of that feature
	 * possible in textbox.
	 */
	struct textline {
		haunted::ui::textbox *box = nullptr;

		textline() {}

		virtual ~textline() = default;

		/** Returns the number of blank spaces at the beginning of a row to use when the line's longer than the width of
		 *  its container and has to be wrapped. The first row of the line isn't padded, but all subsequent rows are. */
		virtual int get_continuation() const = 0;

		/** Returns the text for a given row relative to the line for a given textbox width. */
		virtual std::string text_at_row(size_t width, int row, bool pad_right = true);

		/** Returns the number of rows the line will occupy for a given width. */
		virtual int num_rows(int width);

		/** Called when the line is clicked on. The mouse_report's position is relative to the top left of the line. */
		virtual void on_mouse(const haunted::mouse_report &) {}

		/** Returns the raw text of the line. */
		virtual operator std::string() = 0;

		bool operator==(textline &);
	};

	/**
	 * Represents a line of text with static, unchanging content.
	 */
	struct simpleline: public textline {
		int continuation = 0;

		/** The raw text of the line. */
		std::string text = "";

		simpleline(const std::string &text_, int continuation_ = 0);
		simpleline(int continuation_): continuation(continuation_) {}
		simpleline(): simpleline("", 0) {}

		virtual operator std::string() override;

		int get_continuation() const override { return continuation; }

		virtual bool operator==(const simpleline &);
	};

	/**
	 * Represents a multiline box of text.
	 */
	class textbox: public colored_control {
		friend class haunted::tests::maintest;

		public:
			using line_ptr = std::shared_ptr<textline>;

		protected:
			/** Holds all the textlines in the box. */
			std::deque<line_ptr> lines;

			/** The number of rows the container has been scrolled vertically. */
			int voffset = 0;

			/** Whether the textbox should automatically scroll to keep up with lines added to the bottom. */
			bool autoscroll = false;

			/** Empties the buffer and replaces it with 0-continuation lines from a vector of string. */
			void set_lines(const std::vector<std::string> &);

			/** When a new line is added, it's usually not necessary to completely redraw the component. Instead,
			 *  scrolling the component and printing only the new line is sufficient.
			 *  @param inserted Whether the line has already been inserted into the textbox's collection. */
			void draw_new_line(textline &, bool inserted = false);

			/** Returns the row on which the next line should be drawn or -1 if it's out of bounds. */
			int next_row(int offset_offset = 0);

			/** Returns a pair of the line at a given row (ignoring voffset and zero-based) and the number of rows past
			 *  the start of the line. For example, if the textbox contains one line that occupies a single row and a
			 *  second line that spans 5 rows, then calling this function with 4 will return {lines[1], 3}. */
			std::pair<textline *, int> line_at_row(int);

			/** Returns the string to print on a given row (zero-based) of the textbox. Handles text wrapping and
			 *  scrolling automatically. */
			std::string text_at_row(int, bool pad_right = true);

			/** Performs vertical scrolling for a given number of rows if autoscrolling is enabled and the right
			 *  conditions are met. This should be done after the line is added to the set of lines but before the line
			 *  is drawn. Returns true if this method caused any scrolling.*/
			bool do_scroll(size_t rows);

			/** The cached return value of total_rows(). */
			int total_rows_ = -1;

			/** Marks the cached return value of total_rows() as dirty. */
			void rows_dirty();

		public:
			/** The minimum number of lines that must be visible at the top. */
			unsigned int scroll_buffer = 0;

			/** Constructs a textbox with a parent, a position and initial contents. */
			textbox(container *parent_, position pos_, const std::vector<std::string> &contents_);

			/** Constructs a textbox with a parent and position and empty contents. */
			textbox(container *parent_, position pos_): textbox(parent_, pos_, {}) {}

			/** Constructs a textbox with a parent, initial contents and a default position. */
			textbox(container *parent_, const std::vector<std::string> &contents_);

			/** Constructs a textbox with a parent, a default position and empty contents. */
			textbox(container *parent_): textbox(parent_, std::vector<std::string> {}) {}

			/** Constructs a textbox with no parent and no contents. */
			textbox(): textbox(nullptr, std::vector<std::string> {}) {}

			/** Deletes all lines in the textbox. */
			void clear_lines();

			std::deque<line_ptr> & get_lines() { return lines; }

			/** Scrolls the textbox down (positive argument) or up (negative argument). */
			void vscroll(int = 1);

			/** Returns the vertical offset. */
			int get_voffset() const;

			/** Sets the vertical offset. */
			void set_voffset(int);

			bool get_autoscroll() const { return autoscroll; }

			void set_autoscroll(bool);

			/** Returns the number of rows on the terminal a line of text would occupy. */
			int line_rows(textline &);

			/** Returns the total number of rows occupied by all the lines in the text box. */
			int total_rows();

			/** Draws the textbox on the terminal. */
			void draw() override;

			/** Handles keyboard input. */
			bool on_key(const key &) override;

			/** Calls the clicked textline's on_mouse method. */
			bool on_mouse(const mouse_report &) override;

			/** Handles textbox-related keyboard input. */
			bool default_on_key(const key &);

			bool can_draw() const override;

			void focus() override;

			/** Adds a string to the end of the textbox. */
			textbox & operator+=(const std::string &);
			
			/** Adds a line to the end of the textbox. */
			template <EXTENDS(T, textline)>
			textbox & operator+=(T &line) {
				auto w = formicine::perf.watch("template textbox::operator+=");
				std::unique_ptr<T> line_copy = std::make_unique<T>(line);
				line_copy->box = this;
				autoscroll && do_scroll(line_copy->num_rows(pos.width));
				lines.push_back(std::move(line_copy));
				rows_dirty();
				draw_new_line(*lines.back(), true);
				return *this;
			}

			/** Returns the textbox's contents. */
			operator std::string();

			virtual terminal * get_terminal() override { return term; }
			virtual container * get_parent() const override { return parent; }

			size_t size() const { return lines.size(); }

			friend void swap(textbox &left, textbox &right);
	};
}

#endif
