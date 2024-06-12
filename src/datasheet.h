


inline
bool findDatasheet( RoboconfOptions &rbcOpts, const std::string &name, std::string &foundName, bool quetMode = false )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    return rbcOpts.findDatasheet(name, foundName, quetMode);

#if 0
    if (name.empty())
        return false;

    if (startsWith(name,"http://")) // link to web
    {
        foundName = name;
        return true;
    }

    std:: vector<std::string> dshCheckedLocations; // dshCheckedLocations.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    std::ifstream dsStream;
    if ( !includeSearch( dsStream, name, foundName, makeIncVectorFromFileName(inputFilename), /* <= swap to change lookup order => */ rbcOpts.datasheetPaths, &dshCheckedLocations ) )
    {
        if (!quetMode)
        {
            LOG_WARN_OPT("not-found") << "file '" << name << "' not found\n";
            LOG_MSG("datasheet-not-found") << "Datasheet '" << name << "' not found\n";
            for( auto l : dshCheckedLocations)
                LOG_MSG("datasheet-location") <<"  in '"<<l<<"'\n";
        }
        
        return false;
    }

    std::string str;
    if (std::getline( dsStream, str) )
    {
        trim(str);
        if (startsWith(str,"http://"))
        {
            dsStream.close();
            foundName = str;
            return true;
        }
    }

    dsStream.close();

    return true;
#endif
}



