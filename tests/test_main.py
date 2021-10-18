import pytexture2dstudio


def CompressEtc1():
    with open("img/img.png", mode="rb") as r:
        data = r.read()
    data = pytexture2dstudio.CompressEtc1(data, 0, 0, 0, 1)
    with open("compress/CompressEtc1.ktx", mode="wb") as w:
        w.write(data)
    data = pytexture2dstudio.DecompressEtc1(data, 1024, 1024)
    with open("compress/CompressEtc1.png", mode="wb") as w:
        w.write(data)


def CompressEtc2RGB():
    with open("img/img.png", mode="rb") as r:
        data = r.read()
    data = pytexture2dstudio.CompressEtc2RGB(data, 0, 0, 0, 1)
    with open("compress/CompressEtc2RGB.ktx", mode="wb") as w:
        w.write(data)
    data = pytexture2dstudio.DecompressEtc2(data, 1024, 1024)
    with open("compress/CompressEtc2RGB.png", mode="wb") as w:
        w.write(data)


def CompressEtc2RGBA():
    with open("img/img.png", mode="rb") as r:
        data = r.read()
    data = pytexture2dstudio.CompressEtc2RGBA(data, 0, 0, 0, 1)
    with open("compress/CompressEtc2RGBA.ktx", mode="wb") as w:
        w.write(data)
    data = pytexture2dstudio.DecompressEtc2(data, 1024, 1024)
    with open("compress/CompressEtc2RGBA.png", mode="wb") as w:
        w.write(data)


def CompressAstc():
    with open("img/img.png", mode="rb") as r:
        data = r.read()
    data = pytexture2dstudio.CompressAstc(data, 0, 8, 8, 1, 1)
    with open("compress/CompressAstc.astc", mode="wb") as w:
        w.write(data)
    data = pytexture2dstudio.DecompressAstc(data, 1024, 1024, 8, 8)
    with open("compress/CompressAstc.png", mode="wb") as w:
        w.write(data)


if __name__ == '__main__':
    CompressAstc()
    CompressEtc1()
    CompressEtc2RGB()
    CompressEtc2RGBA()
