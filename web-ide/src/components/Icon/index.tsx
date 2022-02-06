import './icons.css';

export default ({name = "", onClick = () => {},  ...others}) => {
  return (
    <i {...others} onClick={(v) => onClick()} className={'icon-' + name} />
  );
};