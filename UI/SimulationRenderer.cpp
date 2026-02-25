#include "SimulationRenderer.h"

using Edge = std::pair<int, int>;


SimulationRenderer::SimulationRenderer(Renderer& r)
    : r(r) { }



void SimulationRenderer::drawUI(std::unordered_map<MenuID, DropdownMenu<SimulationUICommand>> menu_map) {
    for (auto& m : menu_map) {
        m.second.draw(r.getWindow());
    }
}
