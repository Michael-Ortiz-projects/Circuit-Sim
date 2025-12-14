#include "DropdownMenu.h"

DropdownMenu::DropdownMenu(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size)
	: parentButton(fnt, label, pos, size, UICommand::ToggleMenu) {}

void DropdownMenu::addOption(const std::string& label, UICommand cmd) {

	Button temp(parentButton.font, label, parentButton.box.getPosition(), parentButton.box.getSize(), cmd);
	float textWidth = temp.text.getLocalBounds().width + margin * 2.f;
	if (textWidth > maxWidth)
		maxWidth = textWidth / parentButton.textResolutionFactor;
	options.push_back(std::move(temp));
	updateMenuLayout();
}

bool DropdownMenu::poll(UICommand& outputCommand) {
	if (!open) return false;

	for (auto& option : options) {
		if (option.consumed(outputCommand))
			return true;
	}
	return false;
}


void DropdownMenu::updateMenuLayout() {
	float buttonHeight = parentButton.box.getSize().y;

	float x = parentButton.box.getPosition().x;
	float y_initial = parentButton.box.getPosition().y + buttonHeight;
	float y = y_initial + panelMargin;

	for (auto& opt : options) {
        opt.box.setSize({ maxWidth, buttonHeight });
        opt.box.setPosition(x, y );
        opt.alignTextOnLeft(margin);

        y += buttonHeight + rowGap;
    }

	float panelHeight = options.size() * buttonHeight + (options.size() - 1) * rowGap + panelMargin * 2;
	panel.setPosition(x - panelMargin, y_initial);
	panel.setSize({ maxWidth + panelMargin * 2, panelHeight });

	panel.setFillColor(sf::Color(35, 35, 45));
	panel.setOutlineThickness(1.f);
	panel.setOutlineColor(sf::Color(70, 70, 90));
}

void DropdownMenu::draw(sf::RenderWindow& window) {
	parentButton.draw(window);
	if (open) {
		window.draw(panel);   // draw container first
		for (auto& opt : options)
			opt.draw(window);
	}
}

bool DropdownMenu::onMousePress(const sf::Vector2f& point) {
	bool consumed = false;

	parentButton.onMousePress(point);
	UICommand cmd;
	if (parentButton.consumed(cmd)) {
		consumed = true;
	}

	if (open) {
		for (auto& option : options) {
			option.onMousePress(point);
			if (option.consumed(cmd)) {
				consumed = true;
			}
		}
	}
	return consumed;
}

bool DropdownMenu::onMouseMove(const sf::Vector2f& point) {
	bool consumed = false;

	UICommand cmd;
	if (parentButton.consumed(cmd)) consumed = true;

	if (open) {
		for (auto& option : options) {
			option.onMouseMove(point);
			if (option.consumed(cmd)) consumed = true;
		}
	}

	return consumed;
}

bool DropdownMenu::onMouseRelease(const sf::Vector2f& point) {
	bool consumed = false;

	parentButton.onMouseRelease(point);

	UICommand cmd;

	if (parentButton.consumed(cmd)) {
		if (cmd == UICommand::ToggleMenu) {
			open = !open;

			consumed = true;
		}
	}

	if (open) {
		for (auto& option : options) {
			option.onMouseRelease(point);
			if (option.consumed(cmd)) {
				consumed = true;
			}
		}
	}

	return consumed;
}

bool DropdownMenu::contains(const sf::Vector2f& p) {
	if (parentButton.contains(p)) return true;
	if (open) return panel.getGlobalBounds().contains(p);
	else return false;
}
