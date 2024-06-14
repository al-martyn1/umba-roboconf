
// Разные тесты, которые были в main, но потом стали мешать и захламлять, и оттуда переелали сюда


//----------------------------------------------------------------------------

    {
        auto designatorPinParts = splitDesignatorPinName("U1.100");
        UMBA_USED(designatorPinParts);
         
        int
        cmp = compareDesignatorPinNames("U1.100", "U1.1");
        cmp = compareDesignatorPinNames("aaa", "aaaa");
        cmp = compareDesignatorPinNames("aaa", "aaa");
        cmp = compareDesignatorPinNames("bb", "bb");
         
        UMBA_USED(cmp);

        std:: vector<std::string> names        = { "U34", "Q17", "U23", "DD2", "U26", "U32", "U3", "DD1", "U25", "U31", "U33", "U2", "U1" };
        std:: vector<std::string> sortedNames  = names;
         
        std::sort(sortedNames.begin(), sortedNames.end(), designatorPinNamesLess );
         
        for(auto v: sortedNames)
        {
            std::cout << v << "\n";
        }

    }


//----------------------------------------------------------------------------

    https://ideone.com/EcPCIz
    https://gcc.godbolt.org/z/vfPaTnvTP  - фейлит на MSVC

    // Illustrates how std::vector expands its capacity
    std::size_t vecCapacityStepsCount = 0;
    auto appendReportVectorCapacitySize = [&vecCapacityStepsCount](std::vector<int> &vec, int val)
    {
        std::size_t szBefore  = vec.size();
        std::size_t capBefore = vec.capacity();
        vec.push_back(val);
        std::size_t szAfter   = vec.size();
        std::size_t capAfter  = vec.capacity();
        bool capChanged       = capBefore!=capAfter;
        if (capChanged)
            ++vecCapacityStepsCount;

        // size before/after - szbf/szat
        // capacity before/after - capbf/capat

        std::cout << "V: " << val << ", ";
        // std::cout << "szbf: " << szBefore << ", capbf: " << capBefore << ", ";
        // std::cout << "szat: " << szAfter  << ", capat: " << capAfter  << ", ";
        std::cout << "szbf: "  << szBefore  << ", szat: "  << szAfter << ", ";
        std::cout << "capbf: " << capBefore << ", capat: " << capAfter  << ", ";
        // std::cout << "cap++#: " << vecCapacityStepsCount;
        if (capChanged)
            std::cout << "cap++#: " << vecCapacityStepsCount << " on size: " << szAfter;
        std::cout << "\n";
    };

    std::vector<int> testSizeCapacityVec;
    for(int i=0; i<16; i++)
    {
        appendReportVectorCapacitySize(testSizeCapacityVec, i);
    }
    std::cout << "End of test\n";

    GCC:
        V: 0, szbf: 0, szat: 1, capbf: 0, capat: 1, cap++#: 1 on size: 1
        V: 1, szbf: 1, szat: 2, capbf: 1, capat: 2, cap++#: 2 on size: 2
        V: 2, szbf: 2, szat: 3, capbf: 2, capat: 4, cap++#: 3 on size: 3
        V: 3, szbf: 3, szat: 4, capbf: 4, capat: 4,
        V: 4, szbf: 4, szat: 5, capbf: 4, capat: 8, cap++#: 4 on size: 5
        V: 5, szbf: 5, szat: 6, capbf: 8, capat: 8,
        V: 6, szbf: 6, szat: 7, capbf: 8, capat: 8,
        V: 7, szbf: 7, szat: 8, capbf: 8, capat: 8,
        V: 8, szbf: 8, szat: 9, capbf: 8, capat: 16, cap++#: 5 on size: 9
        V: 9, szbf: 9, szat: 10, capbf: 16, capat: 16,
        V: 10, szbf: 10, szat: 11, capbf: 16, capat: 16,
        V: 11, szbf: 11, szat: 12, capbf: 16, capat: 16,
        V: 12, szbf: 12, szat: 13, capbf: 16, capat: 16,
        V: 13, szbf: 13, szat: 14, capbf: 16, capat: 16,
        V: 14, szbf: 14, szat: 15, capbf: 16, capat: 16,
        V: 15, szbf: 15, szat: 16, capbf: 16, capat: 16,
        End of test

    MSVC:
        V: 0, szbf: 0, szat: 1, capbf: 0, capat: 1, cap++#: 1 on size: 1
        V: 1, szbf: 1, szat: 2, capbf: 1, capat: 2, cap++#: 2 on size: 2
        V: 2, szbf: 2, szat: 3, capbf: 2, capat: 3, cap++#: 3 on size: 3
        V: 3, szbf: 3, szat: 4, capbf: 3, capat: 4, cap++#: 4 on size: 4
        V: 4, szbf: 4, szat: 5, capbf: 4, capat: 6, cap++#: 5 on size: 5
        V: 5, szbf: 5, szat: 6, capbf: 6, capat: 6,
        V: 6, szbf: 6, szat: 7, capbf: 6, capat: 9, cap++#: 6 on size: 7
        V: 7, szbf: 7, szat: 8, capbf: 9, capat: 9,
        V: 8, szbf: 8, szat: 9, capbf: 9, capat: 9,
        V: 9, szbf: 9, szat: 10, capbf: 9, capat: 13, cap++#: 7 on size: 10
        V: 10, szbf: 10, szat: 11, capbf: 13, capat: 13,
        V: 11, szbf: 11, szat: 12, capbf: 13, capat: 13,
        V: 12, szbf: 12, szat: 13, capbf: 13, capat: 13,
        V: 13, szbf: 13, szat: 14, capbf: 13, capat: 19, cap++#: 8 on size: 14
        V: 14, szbf: 14, szat: 15, capbf: 19, capat: 19,
        V: 15, szbf: 15, szat: 16, capbf: 19, capat: 19,
        End of test

    GCC   - на каждом шаге до размера 4 увеличивает вектор. Для небольших векторов с жирными объектами это может быть критично
          - до размера вектора 9 происходит 5 переаллокаций
    MSVC  - на каждом шаге до размера 6 увеличивает вектор. Для небольших векторов с жирными объектами это может быть критично
          - до размера вектора 9 происходит 7 переаллокаций

    Для жирных объектов имеет смысл резервировать место в векторе при каждом добавлении не менее 16 (абс).

//----------------------------------------------------------------------------



