#ifndef FRMREADER_H
#define FRMREADER_H


namespace FRM {
    struct Header {
        char magic[4];
        short version, bones_count;
    };
    struct Bone {

    };
}

class FRMReader
{
private:
    FRM::Header header;
public:
    FRMReader();
    bool LoadFRM(QByteArray stream);
};

#endif // FRMREADER_H
