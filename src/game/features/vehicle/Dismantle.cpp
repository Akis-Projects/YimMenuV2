#include "core/commands/Command.hpp"
#include "game/backend/Self.hpp"

namespace YimMenu::Features
{
    class DismantleVehicle : public Command
    {
        using Command::Command;

        virtual void OnCall() override
        {
            if (Self::GetVehicle())
                Self::GetVehicle().Dismantle();
        }
    };

    static DismantleVehicle _DismantleVehicle{"dismantlevehicle", "Dismantle Vehicle", "Removes all doors from your current vehicle"};
}