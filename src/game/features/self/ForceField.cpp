#include "core/commands/LoopedCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/BoolCommand.hpp"
#include "core/commands/FloatCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/PersonalVehicles.hpp"
#include "game/gta/Pools.hpp"
#include <cmath>

namespace YimMenu::Features
{
    enum ForceFieldMode
    {
        FF_REMOVE = 0,
        FF_PUSH   = 1,
    };

    class ForceField : public LoopedCommand
    {
        using LoopedCommand::LoopedCommand;

        static constexpr float push_strength = 60.f;

        virtual void OnTick() override
        {
            auto selfPed = Self::GetPed();
            auto selfPos = selfPed.GetPosition();
            auto selfVeh = Self::GetVehicle();

            auto radiusCmd  = Commands::GetCommand<FloatCommand>("forcefieldradius"_J);
            auto modeCmd    = Commands::GetCommand<ListCommand>("forcefieldmode"_J);
            auto vehiclesOn = Commands::GetCommand<BoolCommand>("forcefieldvehicles"_J);
            auto objectsOn  = Commands::GetCommand<BoolCommand>("forcefieldobjects"_J);
            auto pedsOn     = Commands::GetCommand<BoolCommand>("forcefieldpeds"_J);
            auto playersOn  = Commands::GetCommand<BoolCommand>("forcefieldplayers"_J);
            auto ignorePvOn = Commands::GetCommand<BoolCommand>("forcefieldignorepv"_J);

            float radius = radiusCmd ? radiusCmd->GetState() : 10.f;
            int mode      = modeCmd ? modeCmd->GetState() : FF_PUSH;

            auto pvHandle = PersonalVehicles::GetCurrentHandle();

            auto applyEffect = [&](Entity ent, rage::fvector3 pos, bool isPed)
            {
                float dx = pos.x - selfPos.x;
                float dy = pos.y - selfPos.y;
                float dz = pos.z - selfPos.z;
                float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                if (distance > radius || distance < 0.01f)
                    return;

                if (!ent.RequestControl(0))
                    return;

                switch (mode)
                {
                case FF_REMOVE:
                    if (isPed)
                        ent.Kill();
                    else
                        ent.Delete();
                    break;

                case FF_PUSH:
                default:
                {
                    float dirX = 0.f, dirY = 0.f, dirZ = 0.f;
                    if (distance > 0.01f)
                    {
                        dirX = dx / distance;
                        dirY = dy / distance;
                        dirZ = dz / distance;
                    }

                    ent.SetVelocity(rage::fvector3{dirX * push_strength, dirY * push_strength, dirZ * push_strength});
                    break;
                }
                }
            };

            if (vehiclesOn && vehiclesOn->GetState())
            {
                for (auto veh : Pools::GetVehicles())
                {
                    if (!veh)
                        continue;

                    if (selfVeh && veh.GetHandle() == selfVeh.GetHandle())
                        continue;

                    if (ignorePvOn && ignorePvOn->GetState() && pvHandle && veh.GetHandle() == pvHandle.GetHandle())
                        continue;

                    applyEffect(veh, veh.GetPosition(), false);
                }
            }

            if (objectsOn && objectsOn->GetState())
            {
                for (auto obj : Pools::GetObjects())
                {
                    if (obj)
                        applyEffect(obj, obj.GetPosition(), false);
                }
            }

            if (pedsOn && pedsOn->GetState())
            {
                for (auto ped : Pools::GetPeds())
                {
                    if (ped && (!ped.IsPlayer() || playersOn) && ped != selfPed)
                    {
                        if (mode == FF_PUSH)
                            ped.SetRagdoll(true);

                        applyEffect(ped, ped.GetPosition(), true);
                    }
                }
            }
        }
    };

    static ForceField _ForceField{"forcefield", "Force Field", "Pushes or removes nearby entities"};

    static ListCommand _ForceFieldMode{"forcefieldmode",
        "Mode",
        "What happens to entities caught in the force field",
        {{FF_REMOVE, "Remove"}, {FF_PUSH, "Push"}},
        FF_PUSH};

    static BoolCommand _ForceFieldVehicles{"forcefieldvehicles", "Vehicles", "Apply the force field to nearby vehicles", true};
    static BoolCommand _ForceFieldObjects{"forcefieldobjects", "Objects", "Apply the force field to nearby objects", true};
    static BoolCommand _ForceFieldPeds{"forcefieldpeds", "Peds", "Apply the force field to nearby peds", true};
    static BoolCommand _ForceFieldPlayers{"forcefieldplayers", "Players", "Apply the force field to nearby players", true};
    static BoolCommand _ForceFieldIgnorePv{"forcefieldignorepv", "Ignore Personal Vehicle", "Don't apply the force field to your personal vehicle", true};
    static FloatCommand _ForceFieldRadius{"forcefieldradius", "Radius", "How far the force field reaches, in meters", 10.f, 200.f, 25.f};
}