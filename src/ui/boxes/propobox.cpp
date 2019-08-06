#include <stdexcept>

#include "core/terminal.h"
#include "ui/boxes/dualbox.h"
#include "ui/boxes/propobox.h"

namespace haunted::ui::boxes {
	propobox::propobox(const position &pos, double ratio): child(nullptr), dualbox(pos), ratio(ratio) {
		DBGFN();
		if (ratio < 0)
			throw std::domain_error("Box ratio cannot be negative");
	}

	propobox::propobox(container *parent, double ratio, control *one, control *two, const position &pos):
	propobox(pos, ratio) {
		DBGFN();
		if (parent != nullptr) {
			parent->add_child(this);
			term = parent->get_terminal();
		}

		for (control *ctrl: {one, two}) {
			ctrl->set_parent(this);
			ctrl->set_terminal(term);
			children.push_back(ctrl);
		}
	}

	void propobox::set_ratio(const double ratio_) {
		if (ratio != ratio_) {
			ratio = ratio_;
			draw();
		}
	}

	void propobox::draw() {
		if (!can_draw())
			return;

		colored::draw();

		auto lock = term->lock_render();
		for (control *child: children)
			child->draw();
	}

	int propobox::size_one() const {
		return get_size() - size_two();
	}

	int propobox::size_two() const {
		return get_size() / (1.0 + ratio);
	}

	int propobox::get_size() const {
		return orientation == box_orientation::horizontal? pos.width : pos.height;
	}
}
