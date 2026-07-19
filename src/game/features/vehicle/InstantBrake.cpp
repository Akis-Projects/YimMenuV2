#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
    class InstantBrake : public LoopedCommand
    {
        using LoopedCommand::LoopedCommand;

        virtual void OnTick() override
        {
            auto veh = Self::GetVehicle();

            if (!veh)
                return;

            if (PAD::IS_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_VEH_HANDBRAKE))
            {
                veh.SetVelocity(Vector3(0.f, 0.f, 0.f));
            }
        }
    };

    static InstantBrake _InstantBrake{"instantbrake", "Instant Brake", "Instantly stops your vehicle when the brake is pressed"};
}