#ifndef hqspre_exceptions_hpp
#define hqspre_exceptions_hpp

namespace hqspre {

/**
 * \brief Exception that is thrown when the formula is recognized to be unsatisfiable.
 */
class UNSATException: public std::exception
{
public:
    explicit UNSATException():
        std::exception(),
        message()
    { }

    /**
     * \brief Creates a new UNSATException with a description of its reason
     * \param msg description of the exception's reaon
     * \param dqbf if not nullptr, statistics about preprocessing are printed
     */
    explicit UNSATException(const char* msg):
        std::exception(), message(msg)
    { }

    /**
     * \brief Creates a new UNSATException with a description of its reason
     * \param msg description of the exception's reaon
     * \param dqbf if not nullptr, statistics about preprocessing are printed
     */
    explicit UNSATException(const std::string& msg):
        std::exception(),
        message(msg)
    { }

    virtual ~UNSATException() noexcept = default;

    /**
     * \brief Returns a description of the reasons why the exception was thrown.
     */
    virtual const char* what() const noexcept override { return message.c_str(); }

private:
    std::string message; ///< Reason for the exception
};


/**
 * \brief Exception that is thrown when the formula is recognized to be satisfiable.
 */
class SATException: public std::exception
{
public:
    explicit SATException():
        std::exception(),
        message()
    { }

    /**
     * \brief Creates a new SATException with a description of its reason
     * \param msg description of the exception's reaon
     * \param dqbf if not nullptr, statistics about preprocessing are printed
     */
    explicit SATException(const char * msg):
        std::exception(),
        message(msg)
    { }

    /**
     * \brief Creates a new SATException with a description of its reason
     * \param msg description of the exception's reaon
     * \param dqbf if not nullptr, statistics about preprocessing are printed
     */
    explicit SATException(const std::string& msg):
        std::exception(),
        message(msg)
    { }

    virtual ~SATException() noexcept = default;

    /**
     * \brief Returns a description of the reasons why the exception was thrown.
     */
    virtual const char* what() const noexcept override { return message.c_str(); }

private:
    std::string message; ///< Reason for the exception
};

} // end namespace hqspre

#endif
