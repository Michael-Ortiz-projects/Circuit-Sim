#pragma once
#include "Button.h"

enum class MenuID {
	Place,
	File,
	Simulation,
	NewSimulation,
	SimulationType,
	Graph,
	GraphY_Axis,
	GraphX_Axis
};

template <typename CommandT>
struct DropdownOption {
	Button<CommandT> button;
	int value;   //index of GraphVariable in simulation for example, but this value is a way to allow the button to store data that varies with each use case
};

template <typename CommandT>
class DropdownMenu {
public:
	Button<CommandT> parentButton;
	std::vector<DropdownOption<CommandT>> options;
	bool open = false;
	sf::RectangleShape panel;
	float panelMargin = 4;
	float rowGap = 3;

	float margin = 4;
	float maxWidth = 10;
	bool active = true;
	int selectedValue = -1;

	DropdownMenu(sf::Font& fnt, const std::string& label, const sf::Vector2f& pos, const sf::Vector2f& size, CommandT cmd)
		: parentButton(fnt, label, pos, size, cmd) {
	}

	void addOption(const std::string& label, CommandT cmd, int value = -1) {

		Button temp(parentButton.font, label, parentButton.box.getPosition(), parentButton.box.getSize(), cmd);
		
		float textWidth = temp.text.getLocalBounds().width + margin * 2.f;
		//std::cout << label << " width " << textWidth << " max width = " << maxWidth << std::endl;

		if (textWidth > maxWidth)
			maxWidth = textWidth;


		DropdownOption opt{ std::move(temp), value };

		options.push_back(std::move(opt));
		updateMenuLayout();
	}

	void setLabel(const std::string& label) {
		parentButton.setName(label);
		std::cout << "setName() ran\n";
	}
	bool onMousePress(const sf::Vector2f& point) {
		parentButton.onMousePress(point);

		if (open) {
			for (auto& option : options)
				option.button.onMousePress(point);
		}

		return contains(point);
	}

	bool onMouseMove(const sf::Vector2f& point) {
		parentButton.onMouseMove(point);

		if (open) {
			for (auto& option : options)
				option.button.onMouseMove(point);
		}

		return contains(point);
	}

	bool onMouseRelease(const sf::Vector2f& point) {
		parentButton.onMouseRelease(point);

		if (open) {
			for (auto& opt : options)
				opt.button.onMouseRelease(point);
		}

		return contains(point);
	}

	bool poll(CommandT& outputCommand) {
		if (parentButton.consumed(outputCommand)) {
			open = !open;
			std::cout << "Menu ";
			open ? std::cout << "Open\n\n" : std::cout << "Closed\n\n";
			return false;
		}

		if (!open) return false;

		for (auto& option : options) {
			if (option.button.consumed(outputCommand)) {
				selectedValue = option.value;
				open = false;
				return true;
			}
		}

		return false;
	}

	void updateMenuLayout() {
		float buttonHeight = parentButton.box.getSize().y;
		std::cout << "button height = " << buttonHeight << "\n";

		float x = parentButton.box.getPosition().x + 5;
		float y_initial = parentButton.box.getPosition().y + buttonHeight;
		float y = y_initial + panelMargin;

		for (auto& opt : options) {
			opt.button.box.setSize({ maxWidth / fontFactor, buttonHeight });
			opt.button.box.setPosition(x, y);
			opt.button.alignTextOnLeft(margin);

			y += buttonHeight + rowGap;
		}

		float panelHeight = options.size() * buttonHeight + (options.size() - 1) * rowGap + panelMargin * 2;
		panel.setPosition(x - panelMargin, y_initial);
		panel.setSize({ maxWidth / fontFactor + panelMargin * 2, panelHeight });

		panel.setFillColor(sf::Color(35, 35, 45));
		panel.setOutlineThickness(1.f);
		panel.setOutlineColor(sf::Color(70, 70, 90));
	}

	void draw(sf::RenderWindow& window) {
		parentButton.draw(window);
		if (open) {
			window.draw(panel);
			for (auto& opt : options)
				opt.button.draw(window);
		}
	}

	bool contains(const sf::Vector2f& p) {
		if (parentButton.contains(p)) return true;
		if (open) return panel.getGlobalBounds().contains(p);
		else return false;
	}

	int getSelectedValue() const {
		return selectedValue;
	}

	DropdownOption<CommandT>& getOption(int optionValue) {
		auto it = std::find_if(options.begin(), options.end(), [&](const DropdownOption<CommandT>& o) {
			return o.value == optionValue;
			});
		if (it == options.end()) std::cout << "dropdownMenu.getOption() option vector did not contain " << optionValue << "\n";
		else {
			return *it;
		}
	}
};

