#pragma once

namespace Lite {

struct Renderable {
    bool visible = true;

    bool is_visible() const;
};

} // namespace Lite
