#ifndef HAUNTED_UI_BOXES_SWAPBOX_H_
#define HAUNTED_UI_BOXES_SWAPBOX_H_

#include <list>

#include "core/defs.h"
#include "ui/boxes/box.h"
#include "ui/container.h"
#include "ui/control.h"

namespace haunted::ui::boxes {
	/**
	 * Represents a box that contains some number of controls. At most one is active at any given time; the others are
	 * kept in memory but aren't drawn.
	 */
	class swapbox: public virtual box {
		protected:
			std::list<control *> controls = {};
			control *active = nullptr;

		public:
			swapbox(container *, const position &, std::initializer_list<control *> = {});

			virtual int max_children() const override { return 1; }
			virtual void resize(const position &) override;
			virtual void draw() override;
	};
}

#endif