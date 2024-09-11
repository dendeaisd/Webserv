/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SyntaxAnalysis.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ramymoussa <ramymoussa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/10 12:06:57 by fgabler           #+#    #+#             */
/*   Updated: 2024/09/04 22:07:17 by ramymoussa       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SyntaxAnalysis.hpp"

#include <string.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <memory>

#include "ExceptionsParsing.hpp"
#include "HttpContext.hpp"
#include "ServerContext.hpp"

  }
}

  else if (_state == StoringStates::HTTPS_CONTEXT &&
  else if (_state == StoringStates::HTTPS_CONTEXT &&
           _bracketStatus[HTTP_BRACKET].size() == 0)
    _state = StoringStates::MAIN_CONTEXT;
           _bracketStatus[SERVER_BRACKET].size() == 0)
    _state = StoringStates::HTTPS_CONTEXT;
           _bracketStatus[LOCATION_BRACKET].size() == 0)
}

}

}

  switch (_state) {
    case StoringStates::MAIN_CONTEXT:
      return (MAIN_BRACKET);
    case StoringStates::HTTPS_CONTEXT:
      return (HTTP_BRACKET);
    case StoringStates::SERVER_CONTEXT:
      return (SERVER_BRACKET);
    case StoringStates::LOCATION_CONTEXT:
      return (LOCATION_BRACKET);
      break;
  }
  return (MAIN_BRACKET);
}
