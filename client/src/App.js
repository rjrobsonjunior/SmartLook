import React, {useEffect, useState} from 'react'

function App()
{
  const [BackendData, setBackendData] = useState([{}])
  
  useEffect(() => 
  {
    fetch("api/").then(
      response => response.json()
    ).then(
      data => (
        setBackendData(data)
      )
    )
  }, [])

  return(
    <div>
      {(typeof BackendData.users === 'undefined') ? (
      <p>Loading...</p>
      ) : (
        BackendData.users.map((user, i) => (
          <p key={i}>{user}</p>
        ))
      )}
    </div> 
  )
}

export default App