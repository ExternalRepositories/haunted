#ifndef HAUNTED_UI_BOXES_HBOX_H_
#define HAUNTED_UI_BOXES_HBOX_H_

#include "core/defs.h"
#include "ui/boxes/box_dual.h"

namespace haunted::ui::boxes {
	/**
	 * Represents a box that holds two children and displays them horizontally.
	 * The first child is on the left, the second on the right.
	 */
	class hbox: public virtual box_dual {
		protected:
			using box_dual::box_dual;

		public:

			virtual void resize(const position &) override;
			virtual int get_size() const override;
	};
}

#endif