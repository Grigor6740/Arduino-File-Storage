const char PAGE_CSS[] PROGMEM = R"=====(
  body {
    background-color: #000000; 
    color: #00FF41; 
    font-family: 'Courier New', Courier, monospace;
    margin: 0;
    padding: 0;
    line-height: 1.6;
  }

  nav {
    display: flex;
    justify-content: space-between; 
    align-items: center;
    padding: 20px;
    border-bottom: 2px solid #00FF41; 
    background-color: #0d0d0d; 
  }

  .nav-title {
    font-size: 20px; 
    font-weight: bold;
    text-transform: uppercase;
    letter-spacing: 2px;
    text-shadow: 0 0 5px #00FF41; 
  }

  .container {
    max-width: 900px;
    margin: 40px auto; 
    padding: 0 20px;
  }

  a {
    color: #00FF41
  }

  /* BUTTONS */
  .btn {
    background: transparent;
    border: 1px solid #00FF41;
    color: #00FF41;
    padding: 10px 15px;
    text-decoration: none;
    text-transform: uppercase;
    font-family: inherit;
    font-weight: bold;
    cursor: pointer;
    transition: 0.2s;
    display: inline-block;
    font-size: 14px;
  }
  .btn:hover { background: #00FF41; color: #000; box-shadow: 0 0 10px #00FF41; }

  .btn-delete { border-color: #FF3333; color: #FF3333; margin-left: 5px; }
  .btn-delete:hover { background: #FF3333; color: #000; box-shadow: 0 0 10px #FF3333; }

  .btn-download { border-color: #00FFFF; color: #00FFFF; }
  .btn-download:hover { background-color: #00FFFF; color: #000; box-shadow: 0 0 10px #00FFFF; }

  /* TABLE */
  table {
    width: 100%;
    border-collapse: collapse;
    border: 1px solid #333;
  }
  th {
    text-align: left;
    padding: 15px;
    border-bottom: 2px solid #00FF41;
    text-transform: uppercase;
    background: #050505;
  }
  td {
    padding: 15px;
    border-bottom: 1px solid #333;
    word-break: break-all;
  }
  tr:hover { background-color: #111; }

  /* ANIMATION */
  .cursor {
    display: inline-block;
    width: 10px;
    height: 20px;
    background-color: #00FF41;
    animation: blink 1s infinite;
    vertical-align: middle;
    margin-left: 5px;
  }
  @keyframes blink {
    0% { opacity: 0; }
    50% { opacity: 1; }
    100% { opacity: 0; }
  }

  /* MOBILE RESPONSIVE */
  @media (max-width: 600px) {
    nav { flex-direction: column; text-align: center; gap: 15px; }
    .container { margin: 20px auto; }
    thead { display: none; } 
    
    table, tbody, tr, td {
      display: block;
      width: 100%;
      box-sizing: border-box;
    }
    tr { margin-bottom: 20px; border: 1px solid #333; padding: 10px; }
    td {
      text-align: right;
      padding-left: 50%;
      position: relative;
      border-bottom: 1px solid #1a1a1a;
    }
    td:nth-of-type(1):before { content: "FILE"; position: absolute; left: 10px; font-weight: bold; opacity: 0.7;}
    td:nth-of-type(2):before { content: "SIZE"; position: absolute; left: 10px; font-weight: bold; opacity: 0.7;}
    td:nth-of-type(3):before { content: "ACTN"; position: absolute; left: 10px; font-weight: bold; opacity: 0.7;}

    .btn { display: block; width: 100%; text-align: center; margin: 5px 0; box-sizing: border-box; }
    .btn-delete { margin-left: 0; }
  }
)=====";

// --- 2. THE HTML (Uses placeholders %PAGE_CSS% and %FILE_LIST%) ---
const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Arduino File Storage</title>
  <link rel="stylesheet" type="text/css" href="/style.css">
</head>
<body>
  <nav>
    <div class="nav-title">
      >_ Arduino File Storage<span class="cursor"></span>
    </div>
    <div>
      <a href='/upload' id="uploadBtn" class="btn">[ UPLOAD ]</a>
    </div>
  </nav>

  <div class="container">
    <table>
      <thead>
        <tr>
          <th>Filename</th>
          <th>Size</th>
          <th>Action</th>
        </tr>
      </thead>
      <tbody>
        %FILE_LIST%
      </tbody>
    </table>
  </div>
</body>
</html>
)=====";

