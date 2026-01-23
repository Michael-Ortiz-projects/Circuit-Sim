#include "DropdownMenu.h"

DropdownMenu::DropdownMenu(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size)
	: parentButton(fnt, label, pos, size, UICommand::ToggleMenu) {}

void DropdownMenu::addOption(const std::string& label, UICommand cmd) {

	Button temp(parentButton.font, label, parentButton.box.getPosition(), parentButton.box.getSize(), cmd);
	float textWidth = temp.text.getLocalBounds().width + margin * 2.f;
	//std::cout << label << " width " << textWidth << " max width = " << maxWidth << std::endl;
	
	if (textWidth > maxWidth)
		maxWidth = textWidth;
	options.push_back(std::move(temp));
	updateMenuLayout();
}

bool DropdownMenu::onMousePress(const sf::Vector2f& point) {
	parentButton.onMousePress(point);

	if (open) {
		for (auto& option : options)
			option.onMousePress(point);
	}

	return contains(point);
}

bool DropdownMenu::onMouseMove(const sf::Vector2f& point) {
	parentButton.onMouseMove(point);

	if (open) {
		for (auto& option : options)
			option.onMouseMove(point);
	}

	return contains(point);
}

bool DropdownMenu::onMouseRelease(const sf::Vector2f& point) {
	parentButton.onMouseRelease(point);

	if (open) {
		for (auto& opt : options)
			opt.onMouseRelease(point);
	}

	return contains(point);
}

bool DropdownMenu::poll(UICommand& outputCommand) {
	if (parentButton.consumed(outputCommand)) {
		if (outputCommand == UICommand::ToggleMenu) {
			open = !open;
			//std::cout << "Menu ";
			//open ? std::cout << "Open\n\n" : std::cout << "Closed\n\n";
			return false;
		}
	}

	if (!open) return false;

	for (auto& option : options) {
		if (option.consumed(outputCommand)) {
			open = false;
			return true;
		}
	}

	return false;
}

void DropdownMenu::updateMenuLayout() {
	float buttonHeight = parentButton.box.getSize().y;

	float x = parentButton.box.getPosition().x;
	float y_initial = parentButton.box.getPosition().y + buttonHeight;
	float y = y_initial + panelMargin;

	for (auto& opt : options) {
		opt.box.setSize({ maxWidth / fontFactor, buttonHeight });
		opt.box.setPosition(x, y);
		opt.alignTextOnLeft(margin);

		y += buttonHeight + rowGap;
	}

	float panelHeight = options.size() * buttonHeight + (options.size() - 1) * rowGap + panelMargin * 2;
	panel.setPosition(x - panelMargin, y_initial);
	panel.setSize({ maxWidth / fontFactor + panelMargin * 2, panelHeight });

	panel.setFillColor(sf::Color(35, 35, 45));
	panel.setOutlineThickness(1.f);
	panel.setOutlineColor(sf::Color(70, 70, 90));
}

void DropdownMenu::draw(sf::RenderWindow& window) {
	parentButton.draw(window);
	if (open) {
		window.draw(panel);
		for (auto& opt : options)
			opt.draw(window);
	}
}

bool DropdownMenu::contains(const sf::Vector2f& p) {
	if (parentButton.contains(p)) return true;
	if (open) return panel.getGlobalBounds().contains(p);
	else return false;
}
