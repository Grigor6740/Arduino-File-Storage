const char CREATE_FOLDER_FORM_CSS[] PROGMEM = R"=====(
    /* --- GLOBAL STYLES --- */
  body {
    background-color: #000000; 
    color: #00FF41; 
    font-family: 'Courier New', Courier, monospace;
    margin: 0;
    padding: 0;
    line-height: 1.6;
  }

  /* --- NAVIGATION --- */
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

  .btn-cancel {
      background: transparent; 
      border: 1px solid #00FF41; 
      color: #00FF41; 
      padding: 10px 20px; 
      font-family: inherit; 
      font-weight: bold; 
      cursor: pointer; 
      text-transform: uppercase;
      text-decoration: none;
      display: inline-block;
  }

  /* --- LAYOUT --- */
  .container {
    max-width: 600px; 
    margin: 80px auto; 
    padding: 40px;
    border: 1px solid #333; 
    text-align: center;
  }

  h2 { margin-top: 0; text-transform: uppercase; }

  /* --- FORM STYLING --- */
  
  input[type="text"] {
    border: 1px dashed #00FF41; /* Dashed looks more 'technical' */
    padding: 20px;
    width: 100%;
    box-sizing: border-box;
    margin-bottom: 20px;
    color: #00FF41;
    font-family: inherit;
    background: #050505;
    text-align: center;
  }

  input[type="submit"] {
    background: transparent;
    border: 2px solid #00FF41; /* Thicker border */
    color: #00FF41;
    padding: 15px 20px;
    text-transform: uppercase;
    font-family: inherit;
    font-weight: bold;
    font-size: 18px; /* Larger text for mobile */
    cursor: pointer;
    transition: 0.2s;
    width: 100%;
    box-sizing: border-box;
  }

  input[type="submit"]:hover {
    background: #00FF41;
    color: #000; 
    box-shadow: 0 0 15px #00FF41; 
  }

  /* --- ANIMATION --- */
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
    0% { opacity: 0; 50% { opacity: 1; } 100% { opacity: 0; }
  }

  /* --- MOBILE ADJUSTMENTS --- */
  @media (max-width: 600px) {
    nav { flex-direction: column; gap: 15px;}
    .container {
      margin: 20px auto;
      width: 90%; /* Fit screen */
      padding: 20px;
      border: none; /* Remove border on small screens to save space */
    }
    input[type="file"] { text-align: left; }
  }
)=====";

const char CRATE_FOLDER_FORM[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Arduino File Storage</title>
  <link rel="stylesheet" type="text/css" href="/create_folder_page.css">
</head>
<body>

  <nav>
    <div class="nav-title">
      >_ CREATE FOLDER<span class="cursor"></span>
    </div>
    <a href="/" class="btn-cancel">
      [ CANCEL ]
    </a>
  </nav>

  <div class="container">
    <h2>Enter Folder Name</h2>
    <form method='POST' action='/create-folder' enctype='multipart/form-data'>
      <label for="message">Folder name:</label>
      <input type='text' name='folder-name'/>
      <br>
      <input type='submit' value='Submit'>
    </form>
  </div>
</body>
</html>
)=====";