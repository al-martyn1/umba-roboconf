


inline
bool findDatasheet( RoboconfOptions &rbcOpts, const std::string &name, std::string &foundName, bool quetMode = false )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    return rbcOpts.findDatasheet(name, foundName, quetMode);

}



