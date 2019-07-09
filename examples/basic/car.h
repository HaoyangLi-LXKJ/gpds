#pragma once

#include <string>
#include "gpds.h"
#include "color.h"

class Car : public Gpds::Serialize {
public:
    std::string manufacturer;
    std::string model;
    int year_of_construction;
    Color color;

    virtual Gpds::Container toContainer() const override
    {
        Gpds::Container c;

        c.addValue("manufacturer", manufacturer);
        c.addValue("model", model);
        c.addValue("year_of_construction", year_of_construction);
        c.addValue("color", color.toContainer());

        return c;
    }

    virtual void fromContainer(const Gpds::Container &c) override
    {
        manufacturer = c.getValue<std::string>("manufacturer");
        model = c.getValue<std::string>("model");
        year_of_construction = c.getValue<int>("year_of_construction");
        color.fromContainer( *c.getValue<Gpds::Container*>( "color" ) );
    }
};
