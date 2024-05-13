#pragma once

#include "roboconf_options.h"

class RoboconfOptionsConfigurator
{
    RoboconfOptions &rbcOpts;

public:

    RoboconfOptionsConfigurator( RoboconfOptions &r ) : rbcOpts(r)
    {
        //rbcOpts.msgDetailLevels.addToDetailGroup( "search", "search" );
        //setDetailDescription

        rbcOpts.msgDetailLevels.addToDetailGroup( "search-lib", "search-lib-log" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "search-lib", "search-lib-details" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "search-lib", "search-lib-summary" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "datasheet", "datasheet-not-found" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "datasheet", "datasheet-location" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "detect", "detect-grp"  );
        rbcOpts.msgDetailLevels.addToDetailGroup( "detect", "detect-ifs"  );
        rbcOpts.msgDetailLevels.addToDetailGroup( "detect", "detect-iface" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "detect", "detect-conn" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "net-chkstp", "net-chkstp-matched" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "net-chkstp", "net-chkstp-not-matched" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "net-dump", "net-dump-cmps" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "net-dump", "net-dump-nets" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "net-dump", "net-dump-pins" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "net-dump", "net-dump-cmn" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "conf-dump", "conf-dump-rules" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "conf-dump", "conf-dump-rules-cls" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "conf-dump-short", "conf-dump-short-rules" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "conf-dump-short", "conf-dump-short-rules-cls" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "data-dump", "data-dump-short" );


        //rbcOpts.msgDetailLevels.addToDetailGroup( "conf", "conf-dump" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "net", "net-chkstp" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "net", "net-dump" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "pkg", "pkg-known" );
        //rbcOpts.msgDetailLevels.addToDetailGroup( "pkg", "pkg-known" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "cls-chk", "cls-chk-matched" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "cls-chk", "cls-chk-not-matched" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "cls-chk", "cls-chk-not-at-all" );
        
        rbcOpts.msgDetailLevels.addToDetailGroup( "cls", "cls-chk" );

        //rbcOpts.msgDetailLevels.addToDetailGroup( "", "" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "search", "search-lib" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "netlist", "netlist-type" );

        rbcOpts.msgDetailLevels.addToDetailGroup( "grp-dump", "grp-dump-bydsg" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "grp-dump", "grp-dump-clust" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "grp-dump", "grp-dump-bysrc" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "grp-dump", "grp-dump-fgrp" );
        
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "search" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "datasheet" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "detect" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "net" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "netlist" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "pkg" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "cls" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "conf-dump" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "conf-dump-short" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "data-dump" );
        rbcOpts.msgDetailLevels.addToDetailGroup( "9", "grp-dump" );
        
    }

}; // class RoboconfOptionsConfigurator

