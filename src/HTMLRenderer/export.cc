/*
 * export.cc
 *
 * Export styles to HTML
 *
 * by WangLu
 * 2012.08.14
 */

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "HTMLRenderer.h"
#include "namespace.h"
#include "config.h"

using boost::algorithm::ifind_first;

void HTMLRenderer::export_remote_font(const FontInfo & info, const string & suffix, const string & fontfileformat, GfxFont * font) 
{
    allcss_fout << format("@font-face{font-family:f%|1$x|;src:url(") % info.id;

    const std::string fn = (format("f%|1$x|") % info.id).str();
    if(param->single_html)
    {
        allcss_fout << "'data:font/" << fontfileformat << ";base64," << base64stream(ifstream(tmp_dir / (fn+suffix), ifstream::binary)) << "'";
    }
    else
    {
        allcss_fout << (fn+suffix);
    }
    

    allcss_fout << format(")format(\"%1%\");}.f%|2$x|{font-family:f%|2$x|;line-height:%3%;}") % fontfileformat % info.id % (info.ascent - info.descent) << endl;
}

static string general_font_family(GfxFont * font)
{
    if(font -> isFixedWidth())
        return "monospace";
    else if (font -> isSerif())
        return "serif";
    else
        return "sans-serif";
}

// TODO: this function is called when some font is unable to process, may use the name there as a hint
void HTMLRenderer::export_remote_default_font(long long fn_id) 
{
    allcss_fout << format(".f%|1$x|{font-family:sans-serif;color:transparent;visibility:hidden;}")%fn_id << endl;
}

void HTMLRenderer::export_local_font(const FontInfo & info, GfxFont * font, const string & original_font_name, const string & cssfont) 
{
    allcss_fout << format(".f%|1$x|{") % info.id;
    allcss_fout << "font-family:" << ((cssfont == "") ? (original_font_name + "," + general_font_family(font)) : cssfont) << ";";

    if(font->isBold() || ifind_first(original_font_name, "bold"))
        allcss_fout << "font-weight:bold;";

    if(ifind_first(original_font_name, "oblique"))
        allcss_fout << "font-style:oblique;";
    else if(font->isItalic() || ifind_first(original_font_name, "italic"))
        allcss_fout << "font-style:italic;";

    allcss_fout << "line-height:" << (info.ascent - info.descent) << ";";

    allcss_fout << "}" << endl;
}

void HTMLRenderer::export_font_size (long long fs_id, double font_size) 
{
    allcss_fout << format(".s%|1$x|{font-size:%2%px;}") % fs_id % font_size << endl;
}

void HTMLRenderer::export_transform_matrix (long long tm_id, const double * tm) 
{
    allcss_fout << format(".t%|1$x|{") % tm_id;

    // always ignore tm[4] and tm[5] because
    // we have already shifted the origin
    
    // TODO: recognize common matices
    for(const string & prefix : {"", "-ms-", "-moz-", "-webkit-", "-o-"})
    {
        // PDF use a different coordinate system from Web
        allcss_fout << prefix << "transform:matrix("
            << tm[0] << ','
            << -tm[1] << ','
            << -tm[2] << ','
            << tm[3] << ',';

        allcss_fout << "0,0);";
        /*
        if(prefix == "-moz-")
            allcss_fout << format("%1%px,%2%px);") % tm[4] % -tm[5];
        else
            allcss_fout << format("%1%,%2%);") % tm[4] % -tm[5];
            */
    }
    allcss_fout << "}" << endl;
}

void HTMLRenderer::export_letter_space (long long ls_id, double letter_space) 
{
    allcss_fout << format(".l%|1$x|{letter-spacing:%2%px;}") % ls_id % letter_space << endl;
}

void HTMLRenderer::export_word_space (long long ws_id, double word_space) 
{
    allcss_fout << format(".w%|1$x|{word-spacing:%2%px;}") % ws_id % word_space << endl;
}

void HTMLRenderer::export_color (long long color_id, const GfxRGB * rgb) 
{
    allcss_fout << format(".c%|1$x|{color:rgb(%2%,%3%,%4%);}") 
        % color_id % (int)colToByte(rgb->r) % (int)colToByte(rgb->g) % (int)colToByte(rgb->b)
        << endl;
}

void HTMLRenderer::export_whitespace (long long ws_id, double ws_width) 
{
    if(ws_width > 0)
        allcss_fout << format("._%|1$x|{display:inline-block;width:%2%px;}") % ws_id % ws_width << endl;
    else
        allcss_fout << format("._%|1$x|{display:inline;margin-left:%2%px;}") % ws_id % ws_width << endl;
}

void HTMLRenderer::export_rise (long long rise_id, double rise) 
{
    allcss_fout << format(".r%|1$x|{top:%2%px;}") % rise_id % (-rise) << endl;
}

