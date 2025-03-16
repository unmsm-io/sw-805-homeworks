#include <iostream>
#include <string>
#include <vector>

class Weapon
{
private:
    std::string name;
    int power;

public:
    Weapon(const std::string &name, int power) : name(name), power(power) {}

    int getPower() const { return power; }

    friend std::ostream &operator<<(std::ostream &os, const Weapon &w)
    {
        return os << w.name << " (" << w.power << ")";
    }
};

class Character
{
protected:
    std::string name;
    int health;

public:
    Character(const std::string &name, int health) : name(name), health(health) {}

    virtual void attack() = 0;
    virtual ~Character() {};
};

class Hunter : public Character
{
    Weapon weapon;

public:
    Hunter(const std::string &name, int health, const Weapon &weapon)
        : Character(name, health), weapon(weapon) {}

    void attack() override
    {
        std::cout << name << " attacks with " << weapon << std::endl;
    }
};

template <typename T>
T maxDamage(const std::vector<T> &weapons)
{
    T maxWep = weapons[0];
    for (const auto &wep : weapons)
    {
        if (wep.getPower() > maxWep.getPower())
            maxWep = wep;
    }
    return maxWep;
}

int main()
{
    Hunter railly("Railly Hugo", 100, {"Dummy Attack", 9999});
    railly.attack();
}