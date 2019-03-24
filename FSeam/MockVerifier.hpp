//
// Created by FyS on 27/06/17.
//

#ifndef FREESOULS_MOCKVERIFIER_HH
#define FREESOULS_MOCKVERIFIER_HH

#include <string>
#include <functional>
#include <memory>
#include <iostream>
#include <map>
#include <any>

namespace FSeam {

    /**
     * @brief TypeTraitsClass with a ClassName alias in order to get the name of the class to mock (used as key for the methods)
     * @note The typetraits specifications are generated by FSeam generator
     * @tparam T type to mock
     */
    template <typename T>
    struct TypeParseTraits {
        static const std::string ClassName;
    };

    /**
     * @brief basic structure that contains description and utilisation of mocked method
     */
    struct MethodCallVerifier {
        std::string _methodName;
        std::size_t _called = 0;
        std::function<void(void*)> _handler;
        std::vector<std::any> _calledData;
    };

    /**
     * @brief Mocking class, it contains all mocked method / save all calls to methods
     * @details A mock verifier instance class is a class that acknowledge all utilisation (method calls) of the mocked class
     *          this class also contains the mocked method (dupped).
     * @todo improve the mocking class to take the arguments and compare them in a verify
     */
    class MockClassVerifier {
    public:
        MockClassVerifier(std::string className) : _className(className) {}

        void invokeDupedMethod(std::string methodName, void *arg = nullptr) {
            std::string key = _className + std::move(methodName);

            if (_verifiers.find(key) != _verifiers.end()) {
                auto dupedMethod = _verifiers.at(key)->_handler;
                if (dupedMethod)
                    dupedMethod(arg);
            }
        }

        /**
         * @brief This method has to be called each time a mocked class is calling a method (in order to register the call)
         *
         * @param className name of the mocked class
         * @param methodName name of the method called
         */
        void methodCall(std::string methodName, std::any &&callingInfo) {
            std::shared_ptr<MethodCallVerifier> methodCallVerifier;
            std::string key = _className + methodName;

            if (_verifiers.find(key) != _verifiers.end())
                methodCallVerifier = _verifiers.at(key);
            else 
                methodCallVerifier = std::make_shared<MethodCallVerifier>();
            methodCallVerifier->_methodName = std::move(methodName);
            methodCallVerifier->_calledData.emplace_back(std::move(callingInfo));
            methodCallVerifier->_called = methodCallVerifier->_calledData.size();
            _verifiers[key] = methodCallVerifier;
        }

        /**
         * @brief This method make it possible to dupe a method in order to have it do what you want.
         *
         * @param T handler type
         * @param className name of the class to mock
         * @param methodName method name to dupe
         * @param handler dupped method
         * @param isComposed if true, keep the current handler and add this new one as composition,
         *                   if false, override the existing handler if any
         *                   set at false by default
         */
        void dupeMethod(std::string methodName, std::function<void(void*)> handler, bool isComposed = false) {
            auto methodCallVerifier = std::make_shared<MethodCallVerifier>();
            std::string key = _className + methodName;

            if (_verifiers.find(key) != _verifiers.end())
                methodCallVerifier = _verifiers.at(key);
            methodCallVerifier->_methodName = std::move(methodName);
            methodCallVerifier->_calledData.clear();
            methodCallVerifier->_called = methodCallVerifier->_calledData.size();
            if (isComposed && methodCallVerifier->_handler) {
                methodCallVerifier->_handler = [handler, methodCallVerifier](void *data){
                    methodCallVerifier->_handler(data);
                    handler(data);
                };
            }
            else
                methodCallVerifier->_handler = handler;
            _verifiers[key] = methodCallVerifier;
        }

        /**
         * @brief Verify if a method has been called under certain conditions (number of times)
         *
         * @param className class name to verify
         * @param methodName method to verify
         * @param times number of times you verify that the mocked method has been called, if no value set, this method
         *        verify you at least have the mocked method called once
         * @return true if the method encounter your conditions (number of times called), false otherwise
         */
        bool verify(std::string methodName, const int times = -1) const {
            std::string key = _className + std::move(methodName);

            if (_verifiers.find(key) == _verifiers.end()) {
                if (times > 0)
                    std::cout << "Verify error for method " << key << ", method never have been called while we expected "
                              << times << " calls" << std::endl;
                return times == 0;
            }
            bool result = ((times <= -1 && _verifiers.at(key)->_called > 0) || (_verifiers.at(key)->_called == times));
            if (!result)
                std::cout << "Verify error for method " << key << ", method has been called "
                          << _verifiers.at(key)->_called << " and not " << times << std::endl;
            return result;
        }

    private:
        std::string _className;
        std::map<std::string, std::shared_ptr<MethodCallVerifier> > _verifiers;
    };

    /**
     * @brief Mocking singleton, this is the main class of FSeam class contains all the mock
     */
    class MockVerifier {
        inline static std::unique_ptr<MockVerifier> inst = nullptr;

    public:
        MockVerifier() = default;
        ~MockVerifier() = default;

        static MockVerifier &instance() {
            if (inst == nullptr) {
                inst = std::make_unique<MockVerifier>();
            };
            return *(inst.get());
        }

        /**
         * @brief Clean the FSeam context of all previously set mock behaviors
         */
        static void cleanUp() {
            inst.reset();
        }

        bool isMockRegistered(const void *mockPtr) {
            return this->_mockedClass.find(mockPtr) != this->_mockedClass.end();
        }

        /**
         * @brief This method get the mock verifier instance class
         *
         * @details Method that retrieve the mock verifier instance class corresponding to the pointer given as parameter
         *
         * @tparam T type to mock
         * @param mockPtr
         * @return the mock verifier instance class, if not referenced yet, create one by calling the ::addMock(T) method
         */
        template <typename T>
        std::shared_ptr<MockClassVerifier> &getMock(const T *mockPtr) {
            if (!isMockRegistered(mockPtr))
                return addMock(mockPtr);
            return this->_mockedClass.at(mockPtr);
        }

        /**
         * @brief This method get the default mock verifier for a class type
         *
         * @details Method that retrieve the mock default verifier instance class corresponding to type given as template parameter
         *
         * @tparam T type to mock
         * @param mockPtr
         * @return the mock verifier instance class, if not referenced yet, create one by calling the ::addMock(T) method
         */
        std::shared_ptr<MockClassVerifier> &getDefaultMock(std::string classMockName) {
            if (this->_defaultMockedClass.find(classMockName) == this->_defaultMockedClass.end())
                return addDefaultMock(classMockName);
            return this->_defaultMockedClass.at(std::move(classMockName));
        }

    private:
        template <typename T>
        std::shared_ptr<MockClassVerifier> &addMock(const T *mockPtr) {
            this->_mockedClass[mockPtr] = std::make_shared<MockClassVerifier>(TypeParseTraits<T>::ClassName);
            return this->_mockedClass.at(mockPtr);
        }
        std::shared_ptr<MockClassVerifier> &addDefaultMock(const std::string &className) {
            this->_defaultMockedClass[className] = std::make_shared<MockClassVerifier>(className);
            return this->_defaultMockedClass.at(className);
        }

    private:
        std::map<const void*, std::shared_ptr<MockClassVerifier> > _mockedClass;
        std::map<std::string, std::shared_ptr<MockClassVerifier> > _defaultMockedClass;
    };

    // ------------------------ Helper free functions --------------------------

    /**
     * @brief This method get the mock verifier instance class
     *
     * @details Method that retrieve the mock verifier instance class corresponding to the pointer given as parameter

     *
     * @tparam T type to mock
     * @param mockPtr
     * @return the mock verifier instance class, if not referenced yet, create one by calling the ::addMock(T) method
     */
    template <typename T>
    std::shared_ptr<MockClassVerifier> &get(const T *mockPtr) {
        return FSeam::MockVerifier::instance().getMock(mockPtr);
    }

    /**
     * @details Get the Default MockClassVerifier correspond to the templated class
     *          This method has to be used in order to 
     * 
     * @tparam T 
     * @return std::shared_ptr<MockClassVerifier>& 
     */
    template <typename T>
    std::shared_ptr<MockClassVerifier> &getDefault() {
        return FSeam::MockVerifier::instance().getDefaultMock(TypeParseTraits<T>::ClassName);
    }

}

#endif //FREESOULS_MOCKVERIFIER_HH
