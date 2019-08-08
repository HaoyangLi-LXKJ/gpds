#pragma once

#include <ostream>
#include <istream>
#include "archiver.h"

namespace tinyxml2
{
    class XMLDocument;
    class XMLElement;
}

namespace Gpds
{
    class GPDS_EXPORT ArchiverXml : public Archiver
    {
    public:
        // Deal with name hiding
        using Archiver::save;
        using Archiver::load;

        const std::string NAMESPACE_PREFIX = "gpds:";

        struct Settings {
            bool printComments;
            bool annotateListCount;
            bool annotateTypes;
            bool prefixAnnotations;
        };

        Settings settings;

        ArchiverXml();
        ArchiverXml( const ArchiverXml& other ) = default;
        ArchiverXml( ArchiverXml&& other ) = default;
        virtual ~ArchiverXml() override = default;

        virtual bool save(std::ostream& stream, const Container& container, const std::string& rootName) const override;
        virtual bool load(std::istream& stream, Container& container, const std::string& rootName) override;

    private:
        void writeEntry(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement& root, const Container& container) const;
        void readEntry(tinyxml2::XMLElement& rootNode, Container& container);
    };

}
