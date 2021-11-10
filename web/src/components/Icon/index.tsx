import './icons.css';

export default ({name = "", ...others}) => {
  return (
    <i {...others} className={'icon-' + name} />
  );
};