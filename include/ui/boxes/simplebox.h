#ifndef HAUNTED_UI_BOXES_SIMPLEBOX_H_
#define HAUNTED_UI_BOXES_SIMPLEBOX_H_

#include "core/defs.h"
#include "ui/container.h"
#include "ui/control.h"

namespace haunted::ui::boxes {
	/**
	 * Represents a simple box that can contain one child. When the box is moved or resized, the child is moved to the
	 * same position as the box but not resized.
	 */
	class simplebox: public virtual container, public virtual control {
		protected:

		public:
			using control::control;
			
			simplebox(const position &pos_): child(nullptr), control(pos_) {}

			virtual int max_children() const override { return 1; }
			virtual void resize(const position &) override;
			virtual void move(int, int) override;
			virtual void draw() override;
			virtual haunted::terminal * get_terminal() override;
	};
}

#endif
