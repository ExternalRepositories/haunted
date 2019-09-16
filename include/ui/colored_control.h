#ifndef HAUNTED_UI_COLORED_CONTROL_H_
#define HAUNTED_UI_COLORED_CONTROL_H_

#include "ui/control.h"
#include "ui/colored.h"

namespace haunted::ui {
	class colored_control: public control, public colored {
		public:
			using control::control;

			using colored::draw;
			virtual void clear_rect() override;
	};
}

#endif