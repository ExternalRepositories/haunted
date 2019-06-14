#include "lib/ansi.h"

#include "core/terminal.h"
#include "ui/control.h"

namespace haunted::ui {
	control::~control() = default;

	void control::resize(const haunted::position &new_pos) {
		// It's up to the caller of resize() to also call draw().
		pos = new_pos;
	}

	void control::focus() {
		term->focus(this);
	}

	void control::jump() {
		pos.jump();
	}

	void control::clear_rect() {
		// There are four different ways to clear text on a row: erase the entire row, erase
		// everything to the left of the cursor, erase everything to the right of the cursor and
		// simply writing spaces. The first is ideal, the next two are okay and the last is the
		// worst option. Which option we can use depends on the left-offset and width of the
		// control.
		formicine::ansi::save();
		if (pos.left == 0) {
			// If we're at the left edge of the screen, we can clear-line if the width is full, or
			// clear-left otherwise.
			if (pos.width == term->get_cols()) {
				for (int i = 0; i < pos.height; ++i) {
					formicine::ansi::jump(0, pos.top + i);
					formicine::ansi::clear_line();
				}
			} else {
				for (int i = 0; i < pos.height; ++i) {
					formicine::ansi::jump(pos.width, pos.top + i);
					formicine::ansi::clear_left();
				}
			}
		} else if (pos.left + pos.width == term->get_cols()) {
			// If we're at the right edge of the screen, we can clear-right.
			for (int i = 0; i < pos.height; ++i) {
				formicine::ansi::jump(pos.left, pos.top + i);
				formicine::ansi::clear_right();
			}
		} else {
			// If the control doesn't reach either end of the screen, we have to print a bunch
			// of spaces.
			std::string spaces(pos.width, ' ');
			for (int i = 0; i < pos.height; ++i) {
				formicine::ansi::jump(pos.left, pos.top + i);
				std::cout << spaces;
			}
		}

		formicine::ansi::restore();
	}

	void control::flush() {
		term->flush();
	}

	bool control::has_focus() const {
		return term->has_focus(this);
	}

	bool control::at_right() const {
		return pos.left + pos.width == term->get_cols();
	}

	bool control::at_left() const {
		return pos.left == 0;
	}
}
